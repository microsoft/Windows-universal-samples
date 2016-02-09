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

    Class DialLaunchArguments

        Public Property VideoId As String

        Public Property PairingCode As String

        Public Property Position As TimeSpan

        Private Sub New()
        End Sub

        Public Shared Function Parse(arguments As String) As DialLaunchArguments
            Try
                Dim dialLaunchArgs As DialLaunchArguments = New DialLaunchArguments()
                Dim argswithkeys As String() = arguments.Split("&"c)
                For Each currentArgWithKey In argswithkeys
                    Dim key As String = currentArgWithKey.Split("="c)(0)
                    Dim val As String = currentArgWithKey.Split("="c)(1)
                    Select key
                        Case "v"
                            dialLaunchArgs.VideoId = val
                            Exit Select
                        Case "t"
                            dialLaunchArgs.Position = TimeSpan.FromTicks(Long.Parse(val))
                            Exit Select
                        Case "pairingCode"
                            dialLaunchArgs.PairingCode = val
                            Exit Select
                    End Select
                Next

                Return dialLaunchArgs
            Catch ex As Exception
                Throw New ArgumentOutOfRangeException(String.Format("Failed to parse DIAL launch arguments: '{0}'", arguments), ex)
            End Try
        End Function
    End Class
End Namespace
