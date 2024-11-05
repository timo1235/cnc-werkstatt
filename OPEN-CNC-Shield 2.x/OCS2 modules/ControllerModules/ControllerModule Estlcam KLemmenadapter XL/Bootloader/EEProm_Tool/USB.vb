Imports System.IO
Partial Public Class Form1

    Private Function Init_USB() As Boolean
        Try : With USB
                If .IsOpen Then .Close()
                .PortName = EDT_Port.Text
                .BaudRate = 1000000
                .Parity = Ports.Parity.None
                .DataBits = 8
                .StopBits = Ports.StopBits.One
                .Open()
                Sleep(50)
                If .IsOpen = N Then Return N
                .DiscardInBuffer() : .DiscardOutBuffer()
                Return Y
            End With : Catch E As Exception : Return N : End Try : End Function

    Public Sub Send_Data(B() As Byte)
        Try
            If USB.IsOpen Then USB.Write(B, 0, B.Length)
        Catch E As Exception : End Try : End Sub

    Private Function Wait_Data(CMD As CMD, Bytes2Read As Int32, Optional Data() As Byte = Nothing, Optional Timeout As Double = 0.5) As Boolean
        Try
            If CMD <> CMD.DUMMY_READ Then Send_Data(Concat_B_A({CMD}, Data))
            Dim D As Date = Now
            Dim I As Int32 = 0
            Do While I < Bytes2Read AndAlso D.Seconds < Timeout
                Dim BTR As Int32 = USB.BytesToRead : If BTR > 0 Then USB.Read(USB_B, I, BTR) : I += BTR
            Loop
            If I = Bytes2Read Then Return Y Else MSG("Bytes requested: " & Bytes2Read & " / Bytes read: " & I & " / Time " & D.Seconds) : Return N
        Catch E As Exception : Return N : End Try : End Function

End Class
