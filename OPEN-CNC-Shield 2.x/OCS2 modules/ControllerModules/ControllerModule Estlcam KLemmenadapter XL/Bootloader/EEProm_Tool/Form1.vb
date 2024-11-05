Imports System.IO
Imports System.Text
Public Class Form1
    Friend WithEvents USB As New Ports.SerialPort
    Public ID As String = ""
    Public SN As String = ""

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Duplicating the controls that are required more than once:
        For Each O As Control In Panel.Controls
            If O.Tag IsNot Nothing Then
                Dim S As String = O.Tag
                Dim Tags() As String = S.Split(";")
                For I As Int32 = 1 To Tags(0) - 1
                    Dim TXT As String = (I + 1).ToString.PadLeft(2, "0") : If UBound(Tags) >= I Then TXT = Tags(I)
                    Dim NM As String = O.Array_Name(I)
                    Dim LO As New Point(5 + (I + 1) * 125, O.Location.Y)
                    Select Case O.GetType
                        Case GetType(Label) : Panel.Controls.Add(New Label() With {.Name = NM, .Text = TXT, .Size = O.Size, .Location = LO})
                        Case GetType(ComboBox)
                            Dim C = New ComboBox() With {.Name = NM, .Size = O.Size, .Location = LO}
                            For Each J As Object In CType(O, ComboBox).Items : C.Items.Add(J) : Next
                            Panel.Controls.Add(C)
                        Case GetType(TextBox) : Panel.Controls.Add(New TextBox() With {.Name = NM, .Size = O.Size, .Location = LO})
                    End Select
                Next
            End If
        Next
        User_String.MaxLength = EEPC.CHK - EEPC.User_String
    End Sub

    Public Sub RW(R As Boolean)
        Ext_Clock.RW_Byte(EEProm(EEPC.Ext_Clock), R)
        USB_ISO.RW_Byte(EEProm(EEPC.USB_ISO), R)
        Autosquare.RW_Byte(EEProm(EEPC.Autosquare), R)
        Handwheel.RW_Byte(EEProm(EEPC.Handwheel), R)

        DIG_Inputs.RW_Byte(EEProm(EEPC.DIG_Input_Count), R)
        RW_Combobox_Byte_Array(DIG_Input_Type_01, EEPC.DIG_Input_Types, 16, R)

        FRQ_Inputs.RW_Byte(EEProm(EEPC.FRQ_Input_Count), R)
        RW_Combobox_Byte_Array(FRQ_Input_Type_01, EEPC.FRQ_Input_Types, 5, R)

        ADC_Inputs.RW_Byte(EEProm(EEPC.ADC_Input_Count), R)
        RW_Combobox_Byte_Array(ADC_Input_Port_01, EEPC.ADC_Input_Ports, 10, R)
        RW_Combobox_Byte_Array(ADC_Input_Type_01, EEPC.ADC_Input_Types, 10, R)
        RW_Textbox_Single_Array(ADC_Input_R_R_01, EEPC.ADC_Input_R_R, 10, R)
        RW_Textbox_Single_Array(ADC_Input_R_I_01, EEPC.ADC_Input_R_I, 10, R)
        RW_Textbox_Single_Array(ADC_Input_R_IU_01, EEPC.ADC_Input_R_IU, 10, R)
        RW_Textbox_Single_Array(ADC_Input_R_RIU_01, EEPC.ADC_Input_R_RIU, 10, R)

        RW_Textbox_Single_Array(V_Ref_01, EEPC.V_Ref_1024, 6, R)

        Outputs.RW_Byte(EEProm(EEPC.Out_Count), R)
        RW_Combobox_Byte_Array(Out_Type_01, EEPC.Out_Types, 20, R)
        RW_Combobox_Byte_Array(Out_Port_01, EEPC.Out_Ports, 20, R)
        RW_Textbox_Byte_Array(Out_Pin_01, EEPC.Out_Pins, 20, R)
        RW_Textbox_UInt16_Array(Out_A_Max_01, EEPC.Out_Amax, 20, R)

        RW_Combobox_Byte_Array(MISC_Out_Type_01, EEPC.Spindle_Out_Type, 4, R)
        RW_Textbox_UInt16_Array(MISC_Out_A_Max_01, EEPC.Spindle_Out_Amax, 4, R)

        RW_Textbox_UInt32_Array(ID_01, EEPC.Manufacturer_ID, 5, R)

        If R Then
            Dim S As String = ""
            For I As Int32 = EEPC.User_String To EEPC.CHK - 1
                If EEProm(I) > 0 AndAlso EEProm(I) < 255 Then S &= Chr(EEProm(I)) Else Exit For
            Next
            User_String.Text = S
        Else
            For I As Int32 = EEPC.User_String To EEPC.CHK : EEProm(I) = 0 : Next
            For I As Int32 = 0 To User_String.Text.Count - 1 : EEProm(I + EEPC.User_String) = Asc(User_String.Text(I)) : Next
            EEProm_Checksum(Y)
        End If
    End Sub

    Private Function BL_Connect() As Boolean
        MSG("Bootloader Connect")
        If Init_USB() = N Then MSG("Port " & EDT_Port.Text & " cannot be opened") : Return N
        If Wait_Data(CMD.Identify, 32) Then ID = USB_Str(0, 32) Else ID = "" : Return N
        MSG("ID: " & ID)
        If Wait_Data(CMD.Read_Page, 512, {1}) Then SN = USB_Hex(256, 3) & USB_Hex(256 + 16, 16) Else SN = "" : Return N
        MSG("SN: " & SN) : MSG("")
        Return Y : End Function

    Public Function EEProm_Checksum(Write As Boolean) As Boolean
        Dim B1 As Int32 = 0
        Dim B2 As Int32 = 0
        For I As Int32 = 0 To EEPC.CHK - 1 : B1 += EEProm(I) : B2 += B1 : Next
        B1 = B1 Mod 256
        B2 = B2 Mod 256
        If Write Then
            EEProm(EEPC.CHK) = B1
            EEProm(EEPC.CHK + 1) = B2
            Return Y
        Else
            Dim S1 As String = Format(EEProm(EEPC.CHK), "X2") & Format(EEProm(EEPC.CHK + 1), "X2")
            Dim S2 As String = Format(B1, "X2") & Format(B2, "X2")
            If S1 = S2 Then MSG("EEProm Checksum: " & S2) Else MSG("EEPROM CHECKSUM IS WRONG / IS: " & S1 & " SHOULD: " & S2)
            Return S1 = S2
        End If
    End Function

    Public Sub Read_Hex(D As String)
        Dim FS As New FileStream(D, FileMode.Open, FileAccess.Read, FileShare.None)
        Dim SR As New StreamReader(FS, Encoding.ASCII)
        Do While Not SR.EndOfStream
            Dim S As String = SR.ReadLine
            If S.Length >= 13 AndAlso S.Substring(7, 2) = "00" Then
                Dim L As Byte = Convert.ToInt32(S.Substring(1, 2), 16)
                Dim A As Int32 = Convert.ToInt32(S.Substring(3, 4), 16)
                For I As Int32 = 0 To Convert.ToInt32(S.Substring(1, 2), 16) - 1 : EEProm(A + I) = Convert.ToInt32(S.Substring(9 + I * 2, 2), 16) : Next
            End If
        Loop
        SR.Close()
        FS.Close()
        RW(Y)
        EEProm_Checksum(N)
    End Sub

    Private Sub BTN_Read_EEProm_Click(sender As Object, e As EventArgs) Handles BTN_Read_EEProm.Click
        If BL_Connect() = N Then MSG("Cannot connect to bootloader...") : Exit Sub
        If Wait_Data(CMD.Read_Page, 512, {0}) Then
            If USB_B(EEPC.Version) <> 1 Then
                MSG("EEProm contains no valid configuration data! (" & USB_B(EEPC.Version) & ")")
            Else
                MSG("EEProm Configuration Version: " & USB_B(EEPC.Version))
                For I As Int32 = 0 To 511 : EEProm(I) = USB_B(I) : Next
                EEProm_Checksum(N)
                RW(Y)
            End If
        Else
            MSG("EEProm Memory cannot be read!")
        End If
        USB.Close()
    End Sub

    Private Sub BTN_Save_EEProm_Click(sender As Object, e As EventArgs) Handles BTN_Save_EEProm.Click
        RW(N)
        If BL_Connect() = N Then Exit Sub
        Dim B(0 To 32) As Byte : For I As Int32 = 0 To 31 : B(I) = Asc(ID(I)) : Next : B(32) = 0
        If Wait_Data(CMD.Write_Page, 1, B) = N OrElse USB_B(0) <> 1 Then Exit Sub
        USB.Write(EEProm, 0, 512)
        If Wait_Data(CMD.DUMMY_READ, 1, Nothing, 20) = N OrElse USB_B(0) <> 2 Then MSG("EEPROM WRITE FAILED!") Else MSG("EEPROM write successful...")
    End Sub

    Private Sub BTN_Read_EEP_File_Click(sender As Object, e As EventArgs) Handles BTN_Read_EEP_File.Click
        Dim DLG As New OpenFileDialog
        DLG.Filter = "Intel Hex|*.eep"
        If DLG.ShowDialog() = DialogResult.OK Then Read_Hex(DLG.FileName)
    End Sub

    Private Sub BTN_Save_EEP_File_Click(sender As Object, e As EventArgs) Handles BTN_Save_EEP_File.Click
        Dim DLG As New SaveFileDialog
        DLG.Filter = "Intel Hex|*.eep"
        If DLG.ShowDialog = DialogResult.OK Then
            RW(N)
            Dim FS As New FileStream(DLG.FileName, FileMode.Create, FileAccess.Write, FileShare.None)
            Dim SW As New StreamWriter(FS, Encoding.ASCII)
            For I As Int32 = 0 To 31
                Dim S As String = ":10" & Format(I * 16, "X4") & "00"
                For J As Int32 = 0 To 15 : S &= Format(EEProm(I * 16 + J), "X2") : Next
                Dim SUM As Int32 = 0 : For J As Int32 = 0 To 19 : SUM += Convert.ToInt32(S.Substring(1 + J * 2, 2), 16) : Next
                Dim CHK As Byte = (-(SUM Xor 255 + 1) And 255)
                S &= Format(CHK, "X2")
                If S.Substring(9, 32) <> "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" Then SW.WriteLine(S) ': Debug.Print(S)
            Next
            SW.WriteLine(":00000001FF")
            SW.Close()
            FS.Close()
        End If
    End Sub

    Private Sub BTN_Preset_Terminal_XL_Click(sender As Object, e As EventArgs) Handles BTN_Preset_Terminal_XL.Click
        Read_Hex(Application.StartupPath & "\..\..\..\EEProm_Terminal_XL.eep")
    End Sub

    Private Sub BTN_Preset_Terminal_M_Click(sender As Object, e As EventArgs) Handles BTN_Preset_Terminal_M.Click
        Read_Hex(Application.StartupPath & "\..\..\..\EEProm_Terminal_M.eep")
    End Sub

    Private Sub BTN_Preset_Terminal_Light_Click(sender As Object, e As EventArgs) Handles BTN_Preset_Terminal_Light.Click
        Read_Hex(Application.StartupPath & "\..\..\..\EEProm_Terminal_Light.eep")
    End Sub

    Private Sub BTN_Preset_LPT_XL_Click(sender As Object, e As EventArgs) Handles BTN_Preset_LPT_XL.Click
        Read_Hex(Application.StartupPath & "\..\..\..\EEProm_LPT_XL.eep")
    End Sub

    Private Sub BTN_Preset_LPT_Light_Click(sender As Object, e As EventArgs) Handles BTN_Preset_LPT_Light.Click
        Read_Hex(Application.StartupPath & "\..\..\..\EEProm_LPT_Light.eep")
    End Sub


End Class
