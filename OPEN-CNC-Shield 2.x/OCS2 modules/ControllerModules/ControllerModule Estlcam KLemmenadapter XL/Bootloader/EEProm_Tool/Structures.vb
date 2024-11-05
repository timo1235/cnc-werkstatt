Imports System.Runtime.InteropServices
Module Structures

    <StructLayout(LayoutKind.Explicit)> Public Structure I64
        <FieldOffset(0)> Dim B As Boolean
        <FieldOffset(0)> Dim U16 As UInt16
        <FieldOffset(0)> Dim I16 As Int16
        <FieldOffset(0)> Dim U32 As UInt32
        <FieldOffset(0)> Dim I32 As Int32
        <FieldOffset(0)> Dim S As Single
        <FieldOffset(0)> Dim U64 As UInt64
        <FieldOffset(0)> Dim I64 As Int64
        <FieldOffset(0)> Dim D As Double
        <FieldOffset(0)> Dim B08 As Byte
        <FieldOffset(1)> Dim B16 As Byte
        <FieldOffset(2)> Dim B24 As Byte
        <FieldOffset(3)> Dim B32 As Byte
        <FieldOffset(4)> Dim B40 As Byte
        <FieldOffset(5)> Dim B48 As Byte
        <FieldOffset(6)> Dim B56 As Byte
        <FieldOffset(7)> Dim B64 As Byte

        Public Sub New(V As Single)
            S = V : End Sub

        Public Sub New(V As UInt16)
            U16 = V : End Sub

        Public Sub New(V As UInt32)
            U32 = V : End Sub

        Public Sub New(B() As Byte, Addr As Int32, C As Int32)
            For I As Int32 = C - 1 To 0 Step -1 : U32 = (U32 << 8) + B(I + Addr) : Next
        End Sub

        Public Function GB(I As Byte) As Byte
            Select Case I
                Case 0 : Return B08
                Case 1 : Return B16
                Case 2 : Return B24
                Case 3 : Return B32
                Case 4 : Return B40
                Case 5 : Return B48
                Case 6 : Return B56
                Case 7 : Return B64
            End Select
            Return 0 : End Function

        Public Sub To_Mem(Addr As Int32, C As Int32)
            For I As Int32 = 0 To C - 1 : EEProm(Addr + I) = GB(I) : Next
        End Sub

    End Structure
End Module
