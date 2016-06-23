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
Imports System.Runtime.Serialization
Imports System.Text
Imports System.Threading.Tasks

Namespace Global.BackgroundAudioShared.Messages

    <DataContract>
    Public Class AppSuspendedMessage

        Public Sub New()
            Me.Timestamp = DateTime.Now
        End Sub

        Public Sub New(timestamp As DateTime)
            Me.Timestamp = timestamp
        End Sub

        <DataMember>
        Public Timestamp As DateTime
    End Class
End Namespace
