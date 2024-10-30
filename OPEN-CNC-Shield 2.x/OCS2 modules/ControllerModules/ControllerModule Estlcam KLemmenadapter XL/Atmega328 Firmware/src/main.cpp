#include <Arduino.h>
#include <Wire.h>

// Pin definitions
#define JOYSTICK_X_PIN     A0
#define JOYSTICK_Y_PIN     A1
#define JOYSTICK_Z_PIN     A2
#define FEEDRATE_PIN       A3
#define ROTATION_SPEED_PIN A6
#define PROGRAM_START_PIN  5
#define MOTOR_START_PIN    4
#define OK_PIN             6

#define DEBUG 5

#define I2C_ADRESS          0x02
#define I2C_CLOCK_FREQUENCY 10000
#define IDENTITY_STRING     "Handrad 0001"
#define SERIALNO_STRING     "ABCD12345678"
#define READ_FRAME_SIZE     9
#define SEND_FRAME_SIZE     15

#define NUM_SAMPLES 3   // Number of readings for averaging the analog inputs
// threshold for error state - if readings are outside this range,
// the error flag is set because the connection to the joystick/poti might be broken
#define ERROR_THRESHOLD 100   // Readings are in the range 0-1023.
#define MID_VALUE       1023 / 2

// Struct for inputs
struct InputData {
    volatile uint16_t rotationSpeed;
    volatile uint16_t feedrate;
    volatile uint16_t joystickX;
    volatile int16_t  joystickY;
    volatile uint16_t joystickZ;
    volatile bool     programStart;
    volatile bool     motorStart;
    volatile bool     ok;
};

InputData inputData;

const uint8_t    CHALLENGE_VAL[] = {0x3B, 0x59, 0xE8, 0x2A, 0xE9, 0xB1, 0xBE, 0xD8, 0x00, 0x00, 0x00, 0x00};
volatile uint8_t rcv_frame[READ_FRAME_SIZE];
uint8_t          send_frame[SEND_FRAME_SIZE];
volatile uint8_t crc;
uint8_t          frame_counter = 0;

// Function definitions
void     RequestEventHandler();
void     ReceiveEventHandler(int rcv_frame_size);
void     set_input_data();
void     read_inputs();
void     set_identity_data();
void     set_challenge_data();
void     set_serialno_data();
uint8_t  crc_calc();
uint16_t readAverageWithErrorCheck(uint8_t pin, const char *inputName, uint16_t &lastValue);

void setup() {
    Wire.begin(I2C_ADRESS);
    Wire.setClock(I2C_CLOCK_FREQUENCY);
    Wire.onRequest(RequestEventHandler);
    Wire.onReceive(ReceiveEventHandler);
    pinMode(PROGRAM_START_PIN, INPUT_PULLUP);
    pinMode(MOTOR_START_PIN, INPUT_PULLUP);
    pinMode(OK_PIN, INPUT_PULLUP);
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Running");
#endif
}

void loop() {
    read_inputs();
    delay(1);
}

void RequestEventHandler() {
    if (rcv_frame[0] != 0) {
        switch (rcv_frame[0]) {
        case 1:
            set_input_data();
            break;

        case 3:
            set_identity_data();
            break;

        case 4:
            set_challenge_data();
            break;

        case 5:
            set_serialno_data();
            break;
        }
        send_frame[12] = rcv_frame[0];
        send_frame[13] = frame_counter++;
        send_frame[14] = crc_calc();
        Wire.write(send_frame, sizeof(send_frame));
    }
#ifdef DEBUG
    // Serial.print("received : ");
    // for (int i = 0; i < READ_FRAME_SIZE; i++) {
    //     Serial.print(rcv_frame[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.print("; sending: ");
    // for (int i = 0; i < SEND_FRAME_SIZE; i++) {
    //     Serial.print(send_frame[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println("");
#endif
}

void ReceiveEventHandler(int rcv_frame_size) {
    if (rcv_frame_size == READ_FRAME_SIZE) {
        for (int i = 0; i < READ_FRAME_SIZE; i++) {
            rcv_frame[i] = Wire.read();
        }
    }
#ifdef DEBUG
    // Serial.println("Receive Request");
#endif
}

void set_input_data() {
    send_frame[0]  = 0xFF;
    send_frame[1]  = 0xFF;
    send_frame[2]  = lowByte(inputData.feedrate);
    send_frame[3]  = highByte(inputData.feedrate);
    send_frame[4]  = lowByte(inputData.rotationSpeed);
    send_frame[5]  = highByte(inputData.rotationSpeed);
    send_frame[6]  = lowByte(inputData.joystickX);
    send_frame[7]  = highByte(inputData.joystickX);
    send_frame[8]  = lowByte(inputData.joystickY);
    send_frame[9]  = highByte(inputData.joystickY);
    send_frame[10] = lowByte(inputData.joystickZ);
    send_frame[11] = highByte(inputData.joystickZ);
}

void read_inputs() {
    inputData.programStart = digitalRead(PROGRAM_START_PIN);
    inputData.motorStart   = digitalRead(MOTOR_START_PIN);
    inputData.ok           = digitalRead(OK_PIN);

    // Analog readings with error handling
    static uint16_t lastRotationSpeed = inputData.rotationSpeed;
    static uint16_t lastFeedrate      = inputData.feedrate;
    static uint16_t lastJoystickX     = inputData.joystickX;
    static uint16_t lastJoystickY     = inputData.joystickY;
    static uint16_t lastJoystickZ     = inputData.joystickZ;

    inputData.rotationSpeed = readAverageWithErrorCheck(ROTATION_SPEED_PIN, "Rotation Speed", lastRotationSpeed) * 64;
    inputData.feedrate      = readAverageWithErrorCheck(FEEDRATE_PIN, "Feedrate", lastFeedrate) * 64;
    inputData.joystickX     = readAverageWithErrorCheck(JOYSTICK_X_PIN, "JoystickX", lastJoystickX) * 64;
    inputData.joystickY     = readAverageWithErrorCheck(JOYSTICK_Y_PIN, "JoystickY", lastJoystickY) * 64;
    inputData.joystickZ     = readAverageWithErrorCheck(JOYSTICK_Z_PIN, "JoystickZ", lastJoystickZ) * 64;
}

// Function to calculate average and check for error state
uint16_t readAverageWithErrorCheck(uint8_t pin, const char *inputName, uint16_t &lastValue) {
    uint16_t samples[NUM_SAMPLES];
    uint32_t sum = 0;

    // Take multiple readings and calculate the average
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = analogRead(pin);
        sum += samples[i];
        delay(1);   // Short delay between readings for more stable values
    }

    uint16_t average = sum / NUM_SAMPLES;

    // Check for deviations
    for (int i = 0; i < NUM_SAMPLES; i++) {
        int16_t deviation = static_cast<int16_t>(samples[i]) - static_cast<int16_t>(average);

        if (abs(deviation) > ERROR_THRESHOLD) {
// Error state
#ifdef DEBUG
            Serial.print("Error: Significant deviation detected in ");
            Serial.println(inputName);
#endif

            // Use strcmp for string comparison
            if (strcmp(inputName, "JoystickX") == 0 || strcmp(inputName, "JoystickY") == 0 || strcmp(inputName, "JoystickZ") == 0) {
                return MID_VALUE;   // Return midpoint for joystick on error
            } else {
                return lastValue;   // Keep last valid value for rotationSpeed and feedrate
            }
        }
    }

    lastValue = average;   // Update last valid value if no error
    return average;
}

void set_identity_data() {
    for (int i = 0; i < 12; i++) {
        send_frame[i] = IDENTITY_STRING[i];
    }
    send_frame[13] = 3;
}

void set_challenge_data() {
    for (int i = 0; i < 12; i++) {
        send_frame[i] = CHALLENGE_VAL[i];
    }
    send_frame[12] = 4;
}

void set_serialno_data() {
    for (int i = 0; i < 10; i++) {
        send_frame[i] = SERIALNO_STRING[i];
    }
    send_frame[10] = 0;
    send_frame[11] = 0;
    send_frame[12] = 5;
}

uint8_t crc_calc() {
    crc = 0;
    for (int i = 0; i < 14; i++) {
        crc ^= send_frame[i];
        crc++;
    }
    return crc;
}