Imports System.Runtime.CompilerServices

Module Extensions

    <Extension()> Public Function Array_Name(O As Control, I As Int32) As String
        Return O.Name.Substring(0, O.Name.Length - 2) & I.CTRL_NR : End Function

    <Extension()> Public Function Seconds(D As Date) As Double
        Return (Now.ToOADate - D.ToOADate) * c_Day_s : End Function

    <Extension()> Public Function Max(A As Int32, B As Int32) As Int32
        If A > B Then Return B Else Return A
    End Function

    <Extension()> Public Function To_Byte(B As Boolean, Optional T As Byte = 1, Optional F As Byte = 255) As Byte 'Default for false is 255 instead of 0 because erased EEProm cells return 255 instead of 0
        If B Then Return T Else Return F
    End Function

    <Extension()> Public Sub RW_Byte(O As CheckBox, ByRef V As Byte, R As Boolean)
        If R Then O.Checked = V = 1 Else V = O.Checked.To_Byte
    End Sub

    <Extension()> Public Sub RW_Byte(O As TextBox, ByRef V As Byte, R As Boolean, Optional Def As Byte = 0)
        Try
            If R Then O.Text = V.ToString Else V = CByte(O.Text)
        Catch EX As Exception : V = Def : O.Text = V.ToString : End Try
    End Sub

    <Extension()> Public Sub RW_Byte(O As ComboBox, ByRef V As Byte, R As Boolean, Optional Def As Byte = 0)
        Try
            If R Then O.SelectedIndex = V Else V = O.SelectedIndex
        Catch EX As Exception : V = Def : O.SelectedIndex = V : End Try
    End Sub

    <Extension()> Public Function CTRL_NR(I As Int32) As String
        Return (I + 1).ToString.PadLeft(2, "0") : End Function

End Module
