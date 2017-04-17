'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks

Namespace Global.ScreenCasting.Util

    Class ApiInformation

        Public Shared Function IsTypePresent(typeName As String) As Boolean
            Try
                Dim picker As Windows.Media.Casting.CastingDevicePicker = New Windows.Media.Casting.CastingDevicePicker()
                Return True
            Catch
                Return False
            End Try
        End Function
    End Class
End Namespace
