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

Namespace Global.BackgroundAudioShared

    ''' <summary>
    ''' Simple representation for songs in a playlist that can be used both for
    ''' data model (across processes) and view model (foreground UI)
    ''' </summary>
    <DataContract>
    Public Class SongModel

        <DataMember>
        Public Property Title As String

        <DataMember>
        Public Property MediaUri As Uri

        <DataMember>
        Public Property AlbumArtUri As Uri
    End Class
End Namespace
