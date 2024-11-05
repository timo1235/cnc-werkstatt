Partial Public Class Form1
    Public Sub RW_Textbox_Byte_Array(O As TextBox, Addr As Int32, Count As Int32, R As Boolean)
        For I As Int32 = 0 To Count - 1
            Dim TXT As TextBox = Find_CTRL(O.Array_Name(I))
            If R = Y Then Dim V As New I64(EEProm, Addr + I * 1, 1) : TXT.Text = V.B08.ToString
            If R = N Then Dim V As New I64(Convert.ToByte(TXT.Text)) : V.To_Mem(Addr + I * 1, 1)
        Next
    End Sub

    Public Sub RW_Textbox_UInt16_Array(O As TextBox, Addr As Int32, Count As Int32, R As Boolean)
        For I As Int32 = 0 To Count - 1
            Dim TXT As TextBox = Find_CTRL(O.Array_Name(I))
            If R = Y Then Dim V As New I64(EEProm, Addr + I * 2, 2) : TXT.Text = V.U16.ToString
            If R = N Then Dim V As New I64(Convert.ToUInt16(TXT.Text)) : V.To_Mem(Addr + I * 2, 2)
        Next
    End Sub

    Public Sub RW_Textbox_UInt32_Array(O As TextBox, Addr As Int32, Count As Int32, R As Boolean)
        For I As Int32 = 0 To Count - 1
            Dim TXT As TextBox = Find_CTRL(O.Array_Name(I))
            If R = Y Then Dim V As New I64(EEProm, Addr + I * 4, 4) : TXT.Text = V.U32.ToString
            If R = N Then Dim V As New I64(Convert.ToUInt32(TXT.Text)) : V.To_Mem(Addr + I * 4, 4)
        Next
    End Sub

    Public Sub RW_Textbox_Single_Array(O As TextBox, Addr As Int32, Count As Int32, R As Boolean)
        For I As Int32 = 0 To Count - 1
            Dim TXT As TextBox = Find_CTRL(O.Array_Name(I))
            If R = Y Then Dim V As New I64(EEProm, Addr + I * 4, 4) : TXT.Text = V.S.ToString
            If R = N Then Dim V As New I64(Convert.ToSingle(TXT.Text)) : V.To_Mem(Addr + I * 4, 4)
        Next
    End Sub

    Public Sub RW_Combobox_Byte_Array(O As ComboBox, Addr As Int32, Count As Int32, R As Boolean)
        For I As Int32 = 0 To Count - 1
            Dim LST As ComboBox = Find_CTRL(O.Array_Name(I))
            LST.RW_Byte(EEProm(Addr + I), R)
        Next
    End Sub


End Class