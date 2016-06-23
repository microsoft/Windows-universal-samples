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
    Public Class TrackChangedMessage

        Public Sub New()
        End Sub

        Public Sub New(trackId As Uri)
            Me.TrackId = trackId
        End Sub

        <DataMember>
        Public TrackId As Uri
    End Class
End Namespace
