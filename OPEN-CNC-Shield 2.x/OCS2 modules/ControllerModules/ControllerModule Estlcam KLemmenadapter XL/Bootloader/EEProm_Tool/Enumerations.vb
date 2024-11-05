Module Enumerations

    Public Enum CMD As Byte
        Exit_Bootloader = 0
        Identify = 1
        Write_Page = 2
        Read_Page = 3
        DUMMY_READ = 255
    End Enum

    Public Enum EEPC
        Version
        Ext_Clock
        USB_ISO
        Autosquare
        Handwheel

        DIG_Input_Count
        DIG_Input_Types

        FRQ_Input_Count = DIG_Input_Types + 16
        FRQ_Input_Types

        ADC_Input_Count = FRQ_Input_Types + 5
        ADC_Input_Ports
        ADC_Input_Types = ADC_Input_Ports + 10
        ADC_Input_R_R = ADC_Input_Types + 10
        ADC_Input_R_I = ADC_Input_R_R + 40
        ADC_Input_R_IU = ADC_Input_R_I + 40
        ADC_Input_R_RIU = ADC_Input_R_IU + 40

        V_Ref_1024 = ADC_Input_R_RIU + 40
        V_Ref_2048 = V_Ref_1024 + 4
        V_Ref_4096 = V_Ref_2048 + 4
        V_Ref_2500 = V_Ref_4096 + 4
        V_Ref_VDD = V_Ref_2500 + 4
        V_Ref_EXT = V_Ref_VDD + 4

        Out_Count = V_Ref_EXT + 4
        Out_Ports
        Out_Pins = Out_Ports + 20
        Out_Types = Out_Pins + 20
        Out_Amax = Out_Types + 20

        Spindle_Out_Type = Out_Amax + 40
        PWM_Out_Type
        MMS_Out_Type
        RPM_Out_Type
        Spindle_Out_Amax
        PWM_Out_Amax = Spindle_Out_Amax + 2
        MMS_Out_Amax = PWM_Out_Amax + 2
        RPM_Out_Amax = MMS_Out_Amax + 2

        Manufacturer_ID = RPM_Out_Amax + 2
        Product_ID = Manufacturer_ID + 4
        URL_Shop_ID = Product_ID + 4
        URL_Manual_ID = URL_Shop_ID + 4
        Presets_ID = URL_Manual_ID + 4

        User_String = Presets_ID + 4


        CHK = 510
        'Last = 511
    End Enum

End Module
