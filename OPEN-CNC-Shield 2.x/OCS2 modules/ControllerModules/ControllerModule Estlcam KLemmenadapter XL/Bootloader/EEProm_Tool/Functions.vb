Module Functions
    Public USB_B(0 To 511) As Byte
    Public EEProm(0 To 511) As Byte

    Public Sub Sleep(Optional Zeit As Int32 = 10)
        Threading.Thread.Sleep(Zeit) : End Sub

    Public Function Concat_B_A(A() As Byte, B() As Byte) As Byte()
        If B Is Nothing Then Return A Else Dim C(A.Length + B.Length - 1) As Byte : A.CopyTo(C, 0) : B.CopyTo(C, A.Length) : Return C
    End Function

    Public Function USB_Str(ST As Int32, C As Int32) As String
        Dim S As String = "" : For I As Int32 = ST To ST + C - 1 : S &= Chr(USB_B(I)) : Next : Return S : End Function

    Public Function USB_Hex(ST As Int32, C As Int32) As String
        Dim S As String = "" : For I As Int32 = ST To ST + C - 1 : S &= Hex(USB_B(I)).PadLeft(2, "0") : Next : Return S : End Function

End Module

Partial Public Class Form1

    Public Sub MSG(S As String)
        Debug.Print(S)
        EDT_State.AppendText(S & NL)
        Refresh()
    End Sub

    Public Function Find_CTRL(N As String) As Control
        Dim A() As Control = Panel.Controls.Find(N, Y)
        If UBound(A) < 0 Then MSG("Find_CTRL """ & N & """  NOT FOUND!") : Return Nothing Else If UBound(A) > 1 Then MSG("Find_CTRL MULTIPLE CONTROLS """ & N & """ FOUND!")
        Return A(0) : End Function

End Class
