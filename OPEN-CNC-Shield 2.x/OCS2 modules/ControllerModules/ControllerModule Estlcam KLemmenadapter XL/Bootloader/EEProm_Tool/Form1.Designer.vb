<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.Ext_Clock = New System.Windows.Forms.CheckBox()
        Me.USB_ISO = New System.Windows.Forms.CheckBox()
        Me.Autosquare = New System.Windows.Forms.CheckBox()
        Me.Handwheel = New System.Windows.Forms.CheckBox()
        Me.DIG_Input_Type_01 = New System.Windows.Forms.ComboBox()
        Me.LBL_DIG_Input_01 = New System.Windows.Forms.Label()
        Me.BTN_Read_EEProm = New System.Windows.Forms.Button()
        Me.BTN_Save_EEProm = New System.Windows.Forms.Button()
        Me.EDT_Port = New System.Windows.Forms.TextBox()
        Me.LBL_Port = New System.Windows.Forms.Label()
        Me.EDT_State = New System.Windows.Forms.TextBox()
        Me.LBL_DIG_Inputs = New System.Windows.Forms.Label()
        Me.DIG_Inputs = New System.Windows.Forms.TextBox()
        Me.LBL_DIG_Input_Types = New System.Windows.Forms.Label()
        Me.ADC_Input_Type_01 = New System.Windows.Forms.ComboBox()
        Me.Panel = New System.Windows.Forms.Panel()
        Me.LBL_ADC_Input_Port = New System.Windows.Forms.Label()
        Me.ADC_Input_Port_01 = New System.Windows.Forms.ComboBox()
        Me.User_String = New System.Windows.Forms.TextBox()
        Me.LBL_User_String = New System.Windows.Forms.Label()
        Me.LBL_ID = New System.Windows.Forms.Label()
        Me.LBL_IDS_01 = New System.Windows.Forms.Label()
        Me.ID_01 = New System.Windows.Forms.TextBox()
        Me.LBL_MISC_Out_A_Max = New System.Windows.Forms.Label()
        Me.MISC_Out_A_Max_01 = New System.Windows.Forms.TextBox()
        Me.LBL_MISTC_Out_Types = New System.Windows.Forms.Label()
        Me.LBL_MISC_OUT_01 = New System.Windows.Forms.Label()
        Me.MISC_Out_Type_01 = New System.Windows.Forms.ComboBox()
        Me.LBL_Out_A_Max = New System.Windows.Forms.Label()
        Me.Out_A_Max_01 = New System.Windows.Forms.TextBox()
        Me.LBL_Out_Pins = New System.Windows.Forms.Label()
        Me.Out_Pin_01 = New System.Windows.Forms.TextBox()
        Me.LBL_Out_Ports = New System.Windows.Forms.Label()
        Me.Out_Port_01 = New System.Windows.Forms.ComboBox()
        Me.LBL_Out_Types = New System.Windows.Forms.Label()
        Me.LBL_Output_01 = New System.Windows.Forms.Label()
        Me.LBL_Outputs = New System.Windows.Forms.Label()
        Me.Outputs = New System.Windows.Forms.TextBox()
        Me.LBL_VRef = New System.Windows.Forms.Label()
        Me.LBL_V_Ref_01 = New System.Windows.Forms.Label()
        Me.V_Ref_01 = New System.Windows.Forms.TextBox()
        Me.LBL_ADC_Input_R_RIU = New System.Windows.Forms.Label()
        Me.ADC_Input_R_RIU_01 = New System.Windows.Forms.TextBox()
        Me.LBL_ADC_Input_R_IU = New System.Windows.Forms.Label()
        Me.ADC_Input_R_IU_01 = New System.Windows.Forms.TextBox()
        Me.LBL_ADC_Input_R_I = New System.Windows.Forms.Label()
        Me.ADC_Input_R_I_01 = New System.Windows.Forms.TextBox()
        Me.LBL_ADC_Input_R_R = New System.Windows.Forms.Label()
        Me.LBL_ADC_Input_Types = New System.Windows.Forms.Label()
        Me.LBL_ADC_Inputs = New System.Windows.Forms.Label()
        Me.ADC_Inputs = New System.Windows.Forms.TextBox()
        Me.LBL_ADC_Input_01 = New System.Windows.Forms.Label()
        Me.FRQ_Input_Type_01 = New System.Windows.Forms.ComboBox()
        Me.LBL_FRQ_Input_Types = New System.Windows.Forms.Label()
        Me.LBL_FRQ_Inputs = New System.Windows.Forms.Label()
        Me.FRQ_Inputs = New System.Windows.Forms.TextBox()
        Me.LBL_FRQ_Input_01 = New System.Windows.Forms.Label()
        Me.Out_Type_01 = New System.Windows.Forms.ComboBox()
        Me.ADC_Input_R_R_01 = New System.Windows.Forms.TextBox()
        Me.BTN_Read_EEP_File = New System.Windows.Forms.Button()
        Me.BTN_Save_EEP_File = New System.Windows.Forms.Button()
        Me.BTN_Preset_Terminal_XL = New System.Windows.Forms.Button()
        Me.BTN_Preset_Terminal_Light = New System.Windows.Forms.Button()
        Me.BTN_Preset_LPT_Light = New System.Windows.Forms.Button()
        Me.BTN_Preset_LPT_XL = New System.Windows.Forms.Button()
        Me.BTN_Preset_Terminal_M = New System.Windows.Forms.Button()
        Me.Panel.SuspendLayout()
        Me.SuspendLayout()
        '
        'Ext_Clock
        '
        Me.Ext_Clock.AutoSize = True
        Me.Ext_Clock.Checked = True
        Me.Ext_Clock.CheckState = System.Windows.Forms.CheckState.Checked
        Me.Ext_Clock.Location = New System.Drawing.Point(5, 5)
        Me.Ext_Clock.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Ext_Clock.Name = "Ext_Clock"
        Me.Ext_Clock.Size = New System.Drawing.Size(722, 17)
        Me.Ext_Clock.TabIndex = 0
        Me.Ext_Clock.Text = "Try to enable external clock (Disable only if PA0 / PA1 pins are used for other f" &
    "unctions / Keep enabled if the oscillator PCB pad is just unpopulated)"
        Me.Ext_Clock.UseVisualStyleBackColor = True
        '
        'USB_ISO
        '
        Me.USB_ISO.AutoSize = True
        Me.USB_ISO.Checked = True
        Me.USB_ISO.CheckState = System.Windows.Forms.CheckState.Checked
        Me.USB_ISO.Location = New System.Drawing.Point(5, 30)
        Me.USB_ISO.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.USB_ISO.Name = "USB_ISO"
        Me.USB_ISO.Size = New System.Drawing.Size(193, 17)
        Me.USB_ISO.TabIndex = 1
        Me.USB_ISO.Text = "USB galvanically isolated (info only)"
        Me.USB_ISO.UseVisualStyleBackColor = True
        '
        'Autosquare
        '
        Me.Autosquare.AutoSize = True
        Me.Autosquare.Checked = True
        Me.Autosquare.CheckState = System.Windows.Forms.CheckState.Checked
        Me.Autosquare.Location = New System.Drawing.Point(5, 55)
        Me.Autosquare.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Autosquare.Name = "Autosquare"
        Me.Autosquare.Size = New System.Drawing.Size(569, 17)
        Me.Autosquare.TabIndex = 2
        Me.Autosquare.Text = "Y-Axis auto square supported (Step YL and YR signals available / LPT Adapter vari" &
    "ants always have this capability)"
        Me.Autosquare.UseVisualStyleBackColor = True
        '
        'Handwheel
        '
        Me.Handwheel.AutoSize = True
        Me.Handwheel.Checked = True
        Me.Handwheel.CheckState = System.Windows.Forms.CheckState.Checked
        Me.Handwheel.Location = New System.Drawing.Point(5, 80)
        Me.Handwheel.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Handwheel.Name = "Handwheel"
        Me.Handwheel.Size = New System.Drawing.Size(277, 17)
        Me.Handwheel.TabIndex = 3
        Me.Handwheel.Text = "Hand wheel pendant available (Mini-DIN6 connector)"
        Me.Handwheel.UseVisualStyleBackColor = True
        '
        'DIG_Input_Type_01
        '
        Me.DIG_Input_Type_01.FormattingEnabled = True
        Me.DIG_Input_Type_01.Items.AddRange(New Object() {"N/A", "TTL", "NPN", "PNP"})
        Me.DIG_Input_Type_01.Location = New System.Drawing.Point(130, 165)
        Me.DIG_Input_Type_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.DIG_Input_Type_01.Name = "DIG_Input_Type_01"
        Me.DIG_Input_Type_01.Size = New System.Drawing.Size(120, 21)
        Me.DIG_Input_Type_01.TabIndex = 6
        Me.DIG_Input_Type_01.Tag = "16"
        '
        'LBL_DIG_Input_01
        '
        Me.LBL_DIG_Input_01.Location = New System.Drawing.Point(130, 145)
        Me.LBL_DIG_Input_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_DIG_Input_01.Name = "LBL_DIG_Input_01"
        Me.LBL_DIG_Input_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_DIG_Input_01.TabIndex = 33
        Me.LBL_DIG_Input_01.Tag = "16"
        Me.LBL_DIG_Input_01.Text = "01"
        '
        'BTN_Read_EEProm
        '
        Me.BTN_Read_EEProm.Location = New System.Drawing.Point(5, 1244)
        Me.BTN_Read_EEProm.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Read_EEProm.Name = "BTN_Read_EEProm"
        Me.BTN_Read_EEProm.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Read_EEProm.TabIndex = 50
        Me.BTN_Read_EEProm.Text = "Read EEProm Configuration from Board"
        Me.BTN_Read_EEProm.UseVisualStyleBackColor = True
        '
        'BTN_Save_EEProm
        '
        Me.BTN_Save_EEProm.Location = New System.Drawing.Point(251, 1244)
        Me.BTN_Save_EEProm.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Save_EEProm.Name = "BTN_Save_EEProm"
        Me.BTN_Save_EEProm.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Save_EEProm.TabIndex = 51
        Me.BTN_Save_EEProm.Text = "Save EEProm Configuration to Board"
        Me.BTN_Save_EEProm.UseVisualStyleBackColor = True
        '
        'EDT_Port
        '
        Me.EDT_Port.Location = New System.Drawing.Point(7, 1207)
        Me.EDT_Port.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.EDT_Port.Name = "EDT_Port"
        Me.EDT_Port.Size = New System.Drawing.Size(127, 20)
        Me.EDT_Port.TabIndex = 52
        Me.EDT_Port.Text = "COM4"
        '
        'LBL_Port
        '
        Me.LBL_Port.AutoSize = True
        Me.LBL_Port.Location = New System.Drawing.Point(142, 1210)
        Me.LBL_Port.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Port.Name = "LBL_Port"
        Me.LBL_Port.Size = New System.Drawing.Size(86, 13)
        Me.LBL_Port.TabIndex = 53
        Me.LBL_Port.Text = "USB / COM Port"
        '
        'EDT_State
        '
        Me.EDT_State.Location = New System.Drawing.Point(8, 1052)
        Me.EDT_State.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.EDT_State.Multiline = True
        Me.EDT_State.Name = "EDT_State"
        Me.EDT_State.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.EDT_State.Size = New System.Drawing.Size(996, 153)
        Me.EDT_State.TabIndex = 54
        '
        'LBL_DIG_Inputs
        '
        Me.LBL_DIG_Inputs.AutoSize = True
        Me.LBL_DIG_Inputs.Location = New System.Drawing.Point(130, 120)
        Me.LBL_DIG_Inputs.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_DIG_Inputs.Name = "LBL_DIG_Inputs"
        Me.LBL_DIG_Inputs.Size = New System.Drawing.Size(471, 13)
        Me.LBL_DIG_Inputs.TabIndex = 56
        Me.LBL_DIG_Inputs.Text = "Digital Input Count (LPT Adapter variants always have at least the 5 inputs at th" &
    "e DB25 connector)"
        '
        'DIG_Inputs
        '
        Me.DIG_Inputs.Location = New System.Drawing.Point(5, 120)
        Me.DIG_Inputs.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.DIG_Inputs.Name = "DIG_Inputs"
        Me.DIG_Inputs.Size = New System.Drawing.Size(120, 20)
        Me.DIG_Inputs.TabIndex = 55
        '
        'LBL_DIG_Input_Types
        '
        Me.LBL_DIG_Input_Types.Location = New System.Drawing.Point(5, 165)
        Me.LBL_DIG_Input_Types.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_DIG_Input_Types.Name = "LBL_DIG_Input_Types"
        Me.LBL_DIG_Input_Types.Size = New System.Drawing.Size(120, 20)
        Me.LBL_DIG_Input_Types.TabIndex = 57
        Me.LBL_DIG_Input_Types.Text = "Input Type:"
        Me.LBL_DIG_Input_Types.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'ADC_Input_Type_01
        '
        Me.ADC_Input_Type_01.FormattingEnabled = True
        Me.ADC_Input_Type_01.Items.AddRange(New Object() {"N/A", "Resistance", "Current 0-20mA", "Voltage 0-10V"})
        Me.ADC_Input_Type_01.Location = New System.Drawing.Point(134, 390)
        Me.ADC_Input_Type_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Input_Type_01.Name = "ADC_Input_Type_01"
        Me.ADC_Input_Type_01.Size = New System.Drawing.Size(120, 21)
        Me.ADC_Input_Type_01.TabIndex = 62
        Me.ADC_Input_Type_01.Tag = "10"
        '
        'Panel
        '
        Me.Panel.AutoScroll = True
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_Port)
        Me.Panel.Controls.Add(Me.ADC_Input_Port_01)
        Me.Panel.Controls.Add(Me.User_String)
        Me.Panel.Controls.Add(Me.LBL_User_String)
        Me.Panel.Controls.Add(Me.LBL_ID)
        Me.Panel.Controls.Add(Me.LBL_IDS_01)
        Me.Panel.Controls.Add(Me.ID_01)
        Me.Panel.Controls.Add(Me.LBL_MISC_Out_A_Max)
        Me.Panel.Controls.Add(Me.MISC_Out_A_Max_01)
        Me.Panel.Controls.Add(Me.LBL_MISTC_Out_Types)
        Me.Panel.Controls.Add(Me.LBL_MISC_OUT_01)
        Me.Panel.Controls.Add(Me.MISC_Out_Type_01)
        Me.Panel.Controls.Add(Me.LBL_Out_A_Max)
        Me.Panel.Controls.Add(Me.Out_A_Max_01)
        Me.Panel.Controls.Add(Me.LBL_Out_Pins)
        Me.Panel.Controls.Add(Me.Out_Pin_01)
        Me.Panel.Controls.Add(Me.LBL_Out_Ports)
        Me.Panel.Controls.Add(Me.Out_Port_01)
        Me.Panel.Controls.Add(Me.LBL_Out_Types)
        Me.Panel.Controls.Add(Me.LBL_Output_01)
        Me.Panel.Controls.Add(Me.LBL_Outputs)
        Me.Panel.Controls.Add(Me.Outputs)
        Me.Panel.Controls.Add(Me.LBL_VRef)
        Me.Panel.Controls.Add(Me.LBL_V_Ref_01)
        Me.Panel.Controls.Add(Me.V_Ref_01)
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_R_RIU)
        Me.Panel.Controls.Add(Me.ADC_Input_R_RIU_01)
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_R_IU)
        Me.Panel.Controls.Add(Me.ADC_Input_R_IU_01)
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_R_I)
        Me.Panel.Controls.Add(Me.ADC_Input_R_I_01)
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_R_R)
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_Types)
        Me.Panel.Controls.Add(Me.LBL_ADC_Inputs)
        Me.Panel.Controls.Add(Me.ADC_Inputs)
        Me.Panel.Controls.Add(Me.LBL_ADC_Input_01)
        Me.Panel.Controls.Add(Me.FRQ_Input_Type_01)
        Me.Panel.Controls.Add(Me.LBL_FRQ_Input_Types)
        Me.Panel.Controls.Add(Me.LBL_FRQ_Inputs)
        Me.Panel.Controls.Add(Me.FRQ_Inputs)
        Me.Panel.Controls.Add(Me.LBL_FRQ_Input_01)
        Me.Panel.Controls.Add(Me.Out_Type_01)
        Me.Panel.Controls.Add(Me.ADC_Input_R_R_01)
        Me.Panel.Controls.Add(Me.Ext_Clock)
        Me.Panel.Controls.Add(Me.ADC_Input_Type_01)
        Me.Panel.Controls.Add(Me.USB_ISO)
        Me.Panel.Controls.Add(Me.Autosquare)
        Me.Panel.Controls.Add(Me.Handwheel)
        Me.Panel.Controls.Add(Me.DIG_Input_Type_01)
        Me.Panel.Controls.Add(Me.LBL_DIG_Input_Types)
        Me.Panel.Controls.Add(Me.LBL_DIG_Inputs)
        Me.Panel.Controls.Add(Me.DIG_Inputs)
        Me.Panel.Controls.Add(Me.LBL_DIG_Input_01)
        Me.Panel.Location = New System.Drawing.Point(0, 0)
        Me.Panel.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Panel.Name = "Panel"
        Me.Panel.Size = New System.Drawing.Size(2400, 1042)
        Me.Panel.TabIndex = 64
        '
        'LBL_ADC_Input_Port
        '
        Me.LBL_ADC_Input_Port.Location = New System.Drawing.Point(5, 360)
        Me.LBL_ADC_Input_Port.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_Port.Name = "LBL_ADC_Input_Port"
        Me.LBL_ADC_Input_Port.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_Port.TabIndex = 231
        Me.LBL_ADC_Input_Port.Text = "Input  Port:"
        Me.LBL_ADC_Input_Port.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'ADC_Input_Port_01
        '
        Me.ADC_Input_Port_01.FormattingEnabled = True
        Me.ADC_Input_Port_01.Items.AddRange(New Object() {"N/A", "D0 / AIN0", "D1 / AIN1", "D2 / AIN2", "D3 / AIN3", "D4 / AIN4", "D5 / AIN5", "D6 / AIN6", "D7 / AIN7", "E0 / AIN8", "E1 / AIN9", "E2 / AIN10", "E3 / AIN11", "E4 / AIN12", "E5 / AIN13", "E6 / AIN14", "E7 / AIN15", "F0 / AIN16", "F1 / AIN17", "F2 / AIN18", "F3 / AIN19", "F4 / AIN20", "F5 / AIN21"})
        Me.ADC_Input_Port_01.Location = New System.Drawing.Point(134, 360)
        Me.ADC_Input_Port_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Input_Port_01.Name = "ADC_Input_Port_01"
        Me.ADC_Input_Port_01.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.ADC_Input_Port_01.Size = New System.Drawing.Size(120, 21)
        Me.ADC_Input_Port_01.TabIndex = 230
        Me.ADC_Input_Port_01.Tag = "10"
        '
        'User_String
        '
        Me.User_String.Location = New System.Drawing.Point(130, 955)
        Me.User_String.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.User_String.Name = "User_String"
        Me.User_String.Size = New System.Drawing.Size(1200, 20)
        Me.User_String.TabIndex = 229
        '
        'LBL_User_String
        '
        Me.LBL_User_String.Location = New System.Drawing.Point(5, 955)
        Me.LBL_User_String.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_User_String.Name = "LBL_User_String"
        Me.LBL_User_String.Size = New System.Drawing.Size(120, 20)
        Me.LBL_User_String.TabIndex = 228
        Me.LBL_User_String.Text = "User String:"
        Me.LBL_User_String.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_ID
        '
        Me.LBL_ID.Location = New System.Drawing.Point(5, 925)
        Me.LBL_ID.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ID.Name = "LBL_ID"
        Me.LBL_ID.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ID.TabIndex = 227
        Me.LBL_ID.Text = "ID:"
        Me.LBL_ID.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_IDS_01
        '
        Me.LBL_IDS_01.Location = New System.Drawing.Point(130, 905)
        Me.LBL_IDS_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_IDS_01.Name = "LBL_IDS_01"
        Me.LBL_IDS_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_IDS_01.TabIndex = 226
        Me.LBL_IDS_01.Tag = "5;Product ID;URL Shop ID;URL Manual ID;Presets ID"
        Me.LBL_IDS_01.Text = "Manufacturer ID"
        '
        'ID_01
        '
        Me.ID_01.Location = New System.Drawing.Point(130, 925)
        Me.ID_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ID_01.Name = "ID_01"
        Me.ID_01.Size = New System.Drawing.Size(120, 20)
        Me.ID_01.TabIndex = 225
        Me.ID_01.Tag = "5"
        '
        'LBL_MISC_Out_A_Max
        '
        Me.LBL_MISC_Out_A_Max.Location = New System.Drawing.Point(6, 860)
        Me.LBL_MISC_Out_A_Max.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_MISC_Out_A_Max.Name = "LBL_MISC_Out_A_Max"
        Me.LBL_MISC_Out_A_Max.Size = New System.Drawing.Size(120, 20)
        Me.LBL_MISC_Out_A_Max.TabIndex = 224
        Me.LBL_MISC_Out_A_Max.Text = "Max Current mA"
        Me.LBL_MISC_Out_A_Max.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'MISC_Out_A_Max_01
        '
        Me.MISC_Out_A_Max_01.Location = New System.Drawing.Point(135, 860)
        Me.MISC_Out_A_Max_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.MISC_Out_A_Max_01.Name = "MISC_Out_A_Max_01"
        Me.MISC_Out_A_Max_01.Size = New System.Drawing.Size(120, 20)
        Me.MISC_Out_A_Max_01.TabIndex = 223
        Me.MISC_Out_A_Max_01.Tag = "4"
        '
        'LBL_MISTC_Out_Types
        '
        Me.LBL_MISTC_Out_Types.Location = New System.Drawing.Point(10, 830)
        Me.LBL_MISTC_Out_Types.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_MISTC_Out_Types.Name = "LBL_MISTC_Out_Types"
        Me.LBL_MISTC_Out_Types.Size = New System.Drawing.Size(120, 20)
        Me.LBL_MISTC_Out_Types.TabIndex = 222
        Me.LBL_MISTC_Out_Types.Text = "Output Type"
        Me.LBL_MISTC_Out_Types.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_MISC_OUT_01
        '
        Me.LBL_MISC_OUT_01.Location = New System.Drawing.Point(130, 810)
        Me.LBL_MISC_OUT_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_MISC_OUT_01.Name = "LBL_MISC_OUT_01"
        Me.LBL_MISC_OUT_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_MISC_OUT_01.TabIndex = 221
        Me.LBL_MISC_OUT_01.Tag = "4;PWM Output;MMS Output;RPM Output"
        Me.LBL_MISC_OUT_01.Text = "Spindle Output"
        '
        'MISC_Out_Type_01
        '
        Me.MISC_Out_Type_01.FormattingEnabled = True
        Me.MISC_Out_Type_01.Items.AddRange(New Object() {"N/A", "TTL 5V", "Relay Driver", "Relay NO", "Relay NC", "Relay NO/NC", "Mosfet NPN", "Mosfet PNP", "Solenoid Driver / Current limited", "0-5V", "0-10V", "0-10V Adjustable", "0-20mA", "0-20mA Adjustable", "4-20mA", "4-20mA Adjustable"})
        Me.MISC_Out_Type_01.Location = New System.Drawing.Point(135, 830)
        Me.MISC_Out_Type_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.MISC_Out_Type_01.Name = "MISC_Out_Type_01"
        Me.MISC_Out_Type_01.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.MISC_Out_Type_01.Size = New System.Drawing.Size(120, 21)
        Me.MISC_Out_Type_01.TabIndex = 220
        Me.MISC_Out_Type_01.Tag = "4"
        '
        'LBL_Out_A_Max
        '
        Me.LBL_Out_A_Max.Location = New System.Drawing.Point(5, 765)
        Me.LBL_Out_A_Max.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Out_A_Max.Name = "LBL_Out_A_Max"
        Me.LBL_Out_A_Max.Size = New System.Drawing.Size(120, 20)
        Me.LBL_Out_A_Max.TabIndex = 219
        Me.LBL_Out_A_Max.Text = "Max Current mA"
        Me.LBL_Out_A_Max.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Out_A_Max_01
        '
        Me.Out_A_Max_01.Location = New System.Drawing.Point(130, 765)
        Me.Out_A_Max_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Out_A_Max_01.Name = "Out_A_Max_01"
        Me.Out_A_Max_01.Size = New System.Drawing.Size(120, 20)
        Me.Out_A_Max_01.TabIndex = 218
        Me.Out_A_Max_01.Tag = "20"
        '
        'LBL_Out_Pins
        '
        Me.LBL_Out_Pins.Location = New System.Drawing.Point(5, 735)
        Me.LBL_Out_Pins.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Out_Pins.Name = "LBL_Out_Pins"
        Me.LBL_Out_Pins.Size = New System.Drawing.Size(120, 20)
        Me.LBL_Out_Pins.TabIndex = 217
        Me.LBL_Out_Pins.Text = "Output Pin:"
        Me.LBL_Out_Pins.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Out_Pin_01
        '
        Me.Out_Pin_01.Location = New System.Drawing.Point(130, 736)
        Me.Out_Pin_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Out_Pin_01.Name = "Out_Pin_01"
        Me.Out_Pin_01.Size = New System.Drawing.Size(120, 20)
        Me.Out_Pin_01.TabIndex = 216
        Me.Out_Pin_01.Tag = "20"
        '
        'LBL_Out_Ports
        '
        Me.LBL_Out_Ports.Location = New System.Drawing.Point(5, 705)
        Me.LBL_Out_Ports.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Out_Ports.Name = "LBL_Out_Ports"
        Me.LBL_Out_Ports.Size = New System.Drawing.Size(120, 20)
        Me.LBL_Out_Ports.TabIndex = 215
        Me.LBL_Out_Ports.Text = "Output Port:"
        Me.LBL_Out_Ports.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Out_Port_01
        '
        Me.Out_Port_01.FormattingEnabled = True
        Me.Out_Port_01.Items.AddRange(New Object() {"N/A", "Port A", "Port B", "Port C", "Port D", "Port E", "Port F", "Port G"})
        Me.Out_Port_01.Location = New System.Drawing.Point(130, 705)
        Me.Out_Port_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Out_Port_01.Name = "Out_Port_01"
        Me.Out_Port_01.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Out_Port_01.Size = New System.Drawing.Size(120, 21)
        Me.Out_Port_01.TabIndex = 214
        Me.Out_Port_01.Tag = "20"
        '
        'LBL_Out_Types
        '
        Me.LBL_Out_Types.Location = New System.Drawing.Point(5, 675)
        Me.LBL_Out_Types.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Out_Types.Name = "LBL_Out_Types"
        Me.LBL_Out_Types.Size = New System.Drawing.Size(120, 20)
        Me.LBL_Out_Types.TabIndex = 213
        Me.LBL_Out_Types.Text = "Output Type"
        Me.LBL_Out_Types.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_Output_01
        '
        Me.LBL_Output_01.Location = New System.Drawing.Point(125, 655)
        Me.LBL_Output_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Output_01.Name = "LBL_Output_01"
        Me.LBL_Output_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_Output_01.TabIndex = 212
        Me.LBL_Output_01.Tag = "20"
        Me.LBL_Output_01.Text = "01"
        '
        'LBL_Outputs
        '
        Me.LBL_Outputs.AutoSize = True
        Me.LBL_Outputs.Location = New System.Drawing.Point(130, 630)
        Me.LBL_Outputs.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_Outputs.Name = "LBL_Outputs"
        Me.LBL_Outputs.Size = New System.Drawing.Size(102, 13)
        Me.LBL_Outputs.TabIndex = 210
        Me.LBL_Outputs.Text = "Digital Output Count"
        '
        'Outputs
        '
        Me.Outputs.Location = New System.Drawing.Point(5, 630)
        Me.Outputs.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Outputs.Name = "Outputs"
        Me.Outputs.Size = New System.Drawing.Size(120, 20)
        Me.Outputs.TabIndex = 209
        '
        'LBL_VRef
        '
        Me.LBL_VRef.Location = New System.Drawing.Point(5, 580)
        Me.LBL_VRef.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_VRef.Name = "LBL_VRef"
        Me.LBL_VRef.Size = New System.Drawing.Size(120, 20)
        Me.LBL_VRef.TabIndex = 208
        Me.LBL_VRef.Text = "Reference Voltage:"
        Me.LBL_VRef.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_V_Ref_01
        '
        Me.LBL_V_Ref_01.Location = New System.Drawing.Point(130, 560)
        Me.LBL_V_Ref_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_V_Ref_01.Name = "LBL_V_Ref_01"
        Me.LBL_V_Ref_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_V_Ref_01.TabIndex = 207
        Me.LBL_V_Ref_01.Tag = "6;2.048V;4.096V;2.500V;VDD;VREFA"
        Me.LBL_V_Ref_01.Text = "1.024V"
        '
        'V_Ref_01
        '
        Me.V_Ref_01.Location = New System.Drawing.Point(130, 580)
        Me.V_Ref_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.V_Ref_01.Name = "V_Ref_01"
        Me.V_Ref_01.Size = New System.Drawing.Size(120, 20)
        Me.V_Ref_01.TabIndex = 206
        Me.V_Ref_01.Tag = "6"
        '
        'LBL_ADC_Input_R_RIU
        '
        Me.LBL_ADC_Input_R_RIU.Location = New System.Drawing.Point(5, 510)
        Me.LBL_ADC_Input_R_RIU.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_R_RIU.Name = "LBL_ADC_Input_R_RIU"
        Me.LBL_ADC_Input_R_RIU.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_R_RIU.TabIndex = 205
        Me.LBL_ADC_Input_R_RIU.Text = "Resistor ""RIU"":"
        Me.LBL_ADC_Input_R_RIU.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'ADC_Input_R_RIU_01
        '
        Me.ADC_Input_R_RIU_01.Location = New System.Drawing.Point(134, 510)
        Me.ADC_Input_R_RIU_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Input_R_RIU_01.Name = "ADC_Input_R_RIU_01"
        Me.ADC_Input_R_RIU_01.Size = New System.Drawing.Size(120, 20)
        Me.ADC_Input_R_RIU_01.TabIndex = 204
        Me.ADC_Input_R_RIU_01.Tag = "10"
        '
        'LBL_ADC_Input_R_IU
        '
        Me.LBL_ADC_Input_R_IU.Location = New System.Drawing.Point(5, 480)
        Me.LBL_ADC_Input_R_IU.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_R_IU.Name = "LBL_ADC_Input_R_IU"
        Me.LBL_ADC_Input_R_IU.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_R_IU.TabIndex = 203
        Me.LBL_ADC_Input_R_IU.Text = "Resistor ""IU"":"
        Me.LBL_ADC_Input_R_IU.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'ADC_Input_R_IU_01
        '
        Me.ADC_Input_R_IU_01.Location = New System.Drawing.Point(134, 480)
        Me.ADC_Input_R_IU_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Input_R_IU_01.Name = "ADC_Input_R_IU_01"
        Me.ADC_Input_R_IU_01.Size = New System.Drawing.Size(120, 20)
        Me.ADC_Input_R_IU_01.TabIndex = 202
        Me.ADC_Input_R_IU_01.Tag = "10"
        '
        'LBL_ADC_Input_R_I
        '
        Me.LBL_ADC_Input_R_I.Location = New System.Drawing.Point(5, 450)
        Me.LBL_ADC_Input_R_I.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_R_I.Name = "LBL_ADC_Input_R_I"
        Me.LBL_ADC_Input_R_I.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_R_I.TabIndex = 201
        Me.LBL_ADC_Input_R_I.Text = "Resistor ""I"":"
        Me.LBL_ADC_Input_R_I.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'ADC_Input_R_I_01
        '
        Me.ADC_Input_R_I_01.Location = New System.Drawing.Point(134, 450)
        Me.ADC_Input_R_I_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Input_R_I_01.Name = "ADC_Input_R_I_01"
        Me.ADC_Input_R_I_01.Size = New System.Drawing.Size(120, 20)
        Me.ADC_Input_R_I_01.TabIndex = 200
        Me.ADC_Input_R_I_01.Tag = "10"
        '
        'LBL_ADC_Input_R_R
        '
        Me.LBL_ADC_Input_R_R.Location = New System.Drawing.Point(5, 420)
        Me.LBL_ADC_Input_R_R.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_R_R.Name = "LBL_ADC_Input_R_R"
        Me.LBL_ADC_Input_R_R.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_R_R.TabIndex = 199
        Me.LBL_ADC_Input_R_R.Text = "Resistor ""R"":"
        Me.LBL_ADC_Input_R_R.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_ADC_Input_Types
        '
        Me.LBL_ADC_Input_Types.Location = New System.Drawing.Point(5, 390)
        Me.LBL_ADC_Input_Types.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_Types.Name = "LBL_ADC_Input_Types"
        Me.LBL_ADC_Input_Types.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_Types.TabIndex = 198
        Me.LBL_ADC_Input_Types.Text = "Input Type:"
        Me.LBL_ADC_Input_Types.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_ADC_Inputs
        '
        Me.LBL_ADC_Inputs.AutoSize = True
        Me.LBL_ADC_Inputs.Location = New System.Drawing.Point(130, 320)
        Me.LBL_ADC_Inputs.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Inputs.Name = "LBL_ADC_Inputs"
        Me.LBL_ADC_Inputs.Size = New System.Drawing.Size(134, 13)
        Me.LBL_ADC_Inputs.TabIndex = 197
        Me.LBL_ADC_Inputs.Text = "Analog Sensor Input Count"
        '
        'ADC_Inputs
        '
        Me.ADC_Inputs.Location = New System.Drawing.Point(5, 320)
        Me.ADC_Inputs.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Inputs.Name = "ADC_Inputs"
        Me.ADC_Inputs.Size = New System.Drawing.Size(120, 20)
        Me.ADC_Inputs.TabIndex = 196
        '
        'LBL_ADC_Input_01
        '
        Me.LBL_ADC_Input_01.Location = New System.Drawing.Point(130, 345)
        Me.LBL_ADC_Input_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_ADC_Input_01.Name = "LBL_ADC_Input_01"
        Me.LBL_ADC_Input_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_ADC_Input_01.TabIndex = 195
        Me.LBL_ADC_Input_01.Tag = "10"
        Me.LBL_ADC_Input_01.Text = "01"
        '
        'FRQ_Input_Type_01
        '
        Me.FRQ_Input_Type_01.FormattingEnabled = True
        Me.FRQ_Input_Type_01.Items.AddRange(New Object() {"N/A", "TTL", "NPN", "PNP"})
        Me.FRQ_Input_Type_01.Location = New System.Drawing.Point(130, 260)
        Me.FRQ_Input_Type_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.FRQ_Input_Type_01.Name = "FRQ_Input_Type_01"
        Me.FRQ_Input_Type_01.Size = New System.Drawing.Size(120, 21)
        Me.FRQ_Input_Type_01.TabIndex = 189
        Me.FRQ_Input_Type_01.Tag = "5"
        '
        'LBL_FRQ_Input_Types
        '
        Me.LBL_FRQ_Input_Types.Location = New System.Drawing.Point(5, 260)
        Me.LBL_FRQ_Input_Types.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_FRQ_Input_Types.Name = "LBL_FRQ_Input_Types"
        Me.LBL_FRQ_Input_Types.Size = New System.Drawing.Size(120, 20)
        Me.LBL_FRQ_Input_Types.TabIndex = 193
        Me.LBL_FRQ_Input_Types.Text = "Input Type:"
        Me.LBL_FRQ_Input_Types.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'LBL_FRQ_Inputs
        '
        Me.LBL_FRQ_Inputs.AutoSize = True
        Me.LBL_FRQ_Inputs.Location = New System.Drawing.Point(130, 215)
        Me.LBL_FRQ_Inputs.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_FRQ_Inputs.Name = "LBL_FRQ_Inputs"
        Me.LBL_FRQ_Inputs.Size = New System.Drawing.Size(368, 13)
        Me.LBL_FRQ_Inputs.TabIndex = 192
        Me.LBL_FRQ_Inputs.Text = "Frequency / Pulse counter Input Count (Currently no more than one possible)"
        '
        'FRQ_Inputs
        '
        Me.FRQ_Inputs.Location = New System.Drawing.Point(5, 215)
        Me.FRQ_Inputs.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.FRQ_Inputs.Name = "FRQ_Inputs"
        Me.FRQ_Inputs.Size = New System.Drawing.Size(120, 20)
        Me.FRQ_Inputs.TabIndex = 191
        '
        'LBL_FRQ_Input_01
        '
        Me.LBL_FRQ_Input_01.Location = New System.Drawing.Point(130, 240)
        Me.LBL_FRQ_Input_01.Margin = New System.Windows.Forms.Padding(4, 0, 4, 0)
        Me.LBL_FRQ_Input_01.Name = "LBL_FRQ_Input_01"
        Me.LBL_FRQ_Input_01.Size = New System.Drawing.Size(120, 20)
        Me.LBL_FRQ_Input_01.TabIndex = 190
        Me.LBL_FRQ_Input_01.Tag = "5"
        Me.LBL_FRQ_Input_01.Text = "01"
        '
        'Out_Type_01
        '
        Me.Out_Type_01.FormattingEnabled = True
        Me.Out_Type_01.Items.AddRange(New Object() {"N/A", "TTL 5V", "Relay Driver", "Relay NO", "Relay NC", "Relay NO/NC", "Mosfet NPN", "Mosfet PNP", "Solenoid Driver / Current limited"})
        Me.Out_Type_01.Location = New System.Drawing.Point(130, 675)
        Me.Out_Type_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Out_Type_01.Name = "Out_Type_01"
        Me.Out_Type_01.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Out_Type_01.Size = New System.Drawing.Size(120, 21)
        Me.Out_Type_01.TabIndex = 148
        Me.Out_Type_01.Tag = "20"
        '
        'ADC_Input_R_R_01
        '
        Me.ADC_Input_R_R_01.Location = New System.Drawing.Point(134, 420)
        Me.ADC_Input_R_R_01.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ADC_Input_R_R_01.Name = "ADC_Input_R_R_01"
        Me.ADC_Input_R_R_01.Size = New System.Drawing.Size(120, 20)
        Me.ADC_Input_R_R_01.TabIndex = 83
        Me.ADC_Input_R_R_01.Tag = "10"
        '
        'BTN_Read_EEP_File
        '
        Me.BTN_Read_EEP_File.Location = New System.Drawing.Point(518, 1244)
        Me.BTN_Read_EEP_File.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Read_EEP_File.Name = "BTN_Read_EEP_File"
        Me.BTN_Read_EEP_File.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Read_EEP_File.TabIndex = 65
        Me.BTN_Read_EEP_File.Text = "Read EEP File"
        Me.BTN_Read_EEP_File.UseVisualStyleBackColor = True
        '
        'BTN_Save_EEP_File
        '
        Me.BTN_Save_EEP_File.Location = New System.Drawing.Point(768, 1244)
        Me.BTN_Save_EEP_File.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Save_EEP_File.Name = "BTN_Save_EEP_File"
        Me.BTN_Save_EEP_File.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Save_EEP_File.TabIndex = 66
        Me.BTN_Save_EEP_File.Text = "Save EEP File"
        Me.BTN_Save_EEP_File.UseVisualStyleBackColor = True
        '
        'BTN_Preset_Terminal_XL
        '
        Me.BTN_Preset_Terminal_XL.Location = New System.Drawing.Point(1012, 1052)
        Me.BTN_Preset_Terminal_XL.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Preset_Terminal_XL.Name = "BTN_Preset_Terminal_XL"
        Me.BTN_Preset_Terminal_XL.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Preset_Terminal_XL.TabIndex = 67
        Me.BTN_Preset_Terminal_XL.Text = "Load Preset Terminal Adapter XL"
        Me.BTN_Preset_Terminal_XL.UseVisualStyleBackColor = True
        '
        'BTN_Preset_Terminal_Light
        '
        Me.BTN_Preset_Terminal_Light.Location = New System.Drawing.Point(1012, 1152)
        Me.BTN_Preset_Terminal_Light.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Preset_Terminal_Light.Name = "BTN_Preset_Terminal_Light"
        Me.BTN_Preset_Terminal_Light.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Preset_Terminal_Light.TabIndex = 68
        Me.BTN_Preset_Terminal_Light.Text = "Load Preset Terminal Adapter Light"
        Me.BTN_Preset_Terminal_Light.UseVisualStyleBackColor = True
        '
        'BTN_Preset_LPT_Light
        '
        Me.BTN_Preset_LPT_Light.Location = New System.Drawing.Point(1258, 1152)
        Me.BTN_Preset_LPT_Light.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Preset_LPT_Light.Name = "BTN_Preset_LPT_Light"
        Me.BTN_Preset_LPT_Light.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Preset_LPT_Light.TabIndex = 70
        Me.BTN_Preset_LPT_Light.Text = "Load Preset LPT Adapter Light"
        Me.BTN_Preset_LPT_Light.UseVisualStyleBackColor = True
        '
        'BTN_Preset_LPT_XL
        '
        Me.BTN_Preset_LPT_XL.Location = New System.Drawing.Point(1258, 1052)
        Me.BTN_Preset_LPT_XL.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Preset_LPT_XL.Name = "BTN_Preset_LPT_XL"
        Me.BTN_Preset_LPT_XL.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Preset_LPT_XL.TabIndex = 69
        Me.BTN_Preset_LPT_XL.Text = "Load Preset LPT Adapter XL"
        Me.BTN_Preset_LPT_XL.UseVisualStyleBackColor = True
        '
        'BTN_Preset_Terminal_M
        '
        Me.BTN_Preset_Terminal_M.Location = New System.Drawing.Point(1012, 1102)
        Me.BTN_Preset_Terminal_M.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.BTN_Preset_Terminal_M.Name = "BTN_Preset_Terminal_M"
        Me.BTN_Preset_Terminal_M.Size = New System.Drawing.Size(238, 40)
        Me.BTN_Preset_Terminal_M.TabIndex = 71
        Me.BTN_Preset_Terminal_M.Text = "Load Preset Terminal Adapter M"
        Me.BTN_Preset_Terminal_M.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None
        Me.ClientSize = New System.Drawing.Size(2400, 1289)
        Me.Controls.Add(Me.BTN_Preset_Terminal_M)
        Me.Controls.Add(Me.BTN_Preset_LPT_Light)
        Me.Controls.Add(Me.BTN_Preset_LPT_XL)
        Me.Controls.Add(Me.BTN_Preset_Terminal_Light)
        Me.Controls.Add(Me.BTN_Preset_Terminal_XL)
        Me.Controls.Add(Me.BTN_Save_EEP_File)
        Me.Controls.Add(Me.BTN_Read_EEP_File)
        Me.Controls.Add(Me.Panel)
        Me.Controls.Add(Me.EDT_State)
        Me.Controls.Add(Me.LBL_Port)
        Me.Controls.Add(Me.EDT_Port)
        Me.Controls.Add(Me.BTN_Save_EEProm)
        Me.Controls.Add(Me.BTN_Read_EEProm)
        Me.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Name = "Form1"
        Me.Text = "Estlcam DB64 Hardware Platform EEProm Configuration Tool"
        Me.Panel.ResumeLayout(False)
        Me.Panel.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents Ext_Clock As CheckBox
    Friend WithEvents USB_ISO As CheckBox
    Friend WithEvents Autosquare As CheckBox
    Friend WithEvents Handwheel As CheckBox
    Friend WithEvents DIG_Input_Type_01 As ComboBox
    Friend WithEvents LBL_DIG_Input_01 As Label
    Friend WithEvents BTN_Read_EEProm As Button
    Friend WithEvents BTN_Save_EEProm As Button
    Friend WithEvents EDT_Port As TextBox
    Friend WithEvents LBL_Port As Label
    Friend WithEvents EDT_State As TextBox
    Friend WithEvents LBL_DIG_Inputs As Label
    Friend WithEvents DIG_Inputs As TextBox
    Friend WithEvents LBL_DIG_Input_Types As Label
    Friend WithEvents ADC_Input_Type_01 As ComboBox
    Friend WithEvents Panel As Panel
    Friend WithEvents ADC_Input_R_R_01 As TextBox
    Friend WithEvents Out_Type_01 As ComboBox
    Friend WithEvents BTN_Read_EEP_File As Button
    Friend WithEvents BTN_Save_EEP_File As Button
    Friend WithEvents FRQ_Input_Type_01 As ComboBox
    Friend WithEvents LBL_FRQ_Input_Types As Label
    Friend WithEvents LBL_FRQ_Inputs As Label
    Friend WithEvents FRQ_Inputs As TextBox
    Friend WithEvents LBL_FRQ_Input_01 As Label
    Friend WithEvents LBL_ADC_Input_Types As Label
    Friend WithEvents LBL_ADC_Inputs As Label
    Friend WithEvents ADC_Inputs As TextBox
    Friend WithEvents LBL_ADC_Input_01 As Label
    Friend WithEvents LBL_ADC_Input_R_R As Label
    Friend WithEvents LBL_ADC_Input_R_RIU As Label
    Friend WithEvents ADC_Input_R_RIU_01 As TextBox
    Friend WithEvents LBL_ADC_Input_R_IU As Label
    Friend WithEvents ADC_Input_R_IU_01 As TextBox
    Friend WithEvents LBL_ADC_Input_R_I As Label
    Friend WithEvents ADC_Input_R_I_01 As TextBox
    Friend WithEvents LBL_V_Ref_01 As Label
    Friend WithEvents V_Ref_01 As TextBox
    Friend WithEvents LBL_VRef As Label
    Friend WithEvents LBL_Out_Types As Label
    Friend WithEvents LBL_Output_01 As Label
    Friend WithEvents LBL_Outputs As Label
    Friend WithEvents Outputs As TextBox
    Friend WithEvents LBL_Out_Pins As Label
    Friend WithEvents LBL_Out_Ports As Label
    Friend WithEvents Out_Port_01 As ComboBox
    Friend WithEvents LBL_Out_A_Max As Label
    Friend WithEvents Out_A_Max_01 As TextBox
    Friend WithEvents Out_Pin_01 As TextBox
    Friend WithEvents LBL_MISC_Out_A_Max As Label
    Friend WithEvents MISC_Out_A_Max_01 As TextBox
    Friend WithEvents LBL_MISTC_Out_Types As Label
    Friend WithEvents LBL_MISC_OUT_01 As Label
    Friend WithEvents MISC_Out_Type_01 As ComboBox
    Friend WithEvents LBL_ID As Label
    Friend WithEvents LBL_IDS_01 As Label
    Friend WithEvents ID_01 As TextBox
    Friend WithEvents LBL_User_String As Label
    Friend WithEvents User_String As TextBox
    Friend WithEvents LBL_ADC_Input_Port As Label
    Friend WithEvents ADC_Input_Port_01 As ComboBox
    Friend WithEvents BTN_Preset_Terminal_XL As Button
    Friend WithEvents BTN_Preset_Terminal_Light As Button
    Friend WithEvents BTN_Preset_LPT_Light As Button
    Friend WithEvents BTN_Preset_LPT_XL As Button
    Friend WithEvents BTN_Preset_Terminal_M As Button
End Class
