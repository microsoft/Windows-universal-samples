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
Imports System.Diagnostics
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.Foundation.Collections
Imports Windows.Media.Playback

Namespace Global.BackgroundAudioShared.Messages

    ''' <summary>
    ''' MessageService makes it easy to send strongly typed messages
    ''' between the foreground and background processes.
    ''' </summary>
    ''' <remarks>
    ''' JSON is used as the underlying serialization mechanism,
    ''' but you don't need to know JSON formatting to create new
    ''' messages.
    ''' 
    ''' See some of the related Message implementations which are
    ''' simple data objects serialized through the standard DataContract
    ''' interface.
    ''' </remarks>
    Public Module MessageService

        ' The underlying BMP methods can pass a ValueSet. MessageService
        ' relies on this to pass a type and body payload.
        Const MessageType As String = "MessageType"

        Const MessageBody As String = "MessageBody"

        Public Sub SendMessageToForeground(Of T)(message As T)
            Dim payload = New ValueSet()
            payload.Add(MessageService.MessageType, GetType(T).FullName)
            payload.Add(MessageService.MessageBody, JsonHelper.ToJson(message))
            BackgroundMediaPlayer.SendMessageToForeground(payload)
        End Sub

        Public Sub SendMessageToBackground(Of T)(message As T)
            Dim payload = New ValueSet()
            payload.Add(MessageService.MessageType, GetType(T).FullName)
            payload.Add(MessageService.MessageBody, JsonHelper.ToJson(message))
            BackgroundMediaPlayer.SendMessageToBackground(payload)
        End Sub

        Public Function TryParseMessage(Of T)(valueSet As ValueSet, ByRef message As T) As Boolean
            Dim messageTypeValue As Object = Nothing
            Dim messageBodyValue As Object = Nothing
            message = Nothing
            If valueSet.TryGetValue(MessageService.MessageType, messageTypeValue) AndAlso valueSet.TryGetValue(MessageService.MessageBody, messageBodyValue) Then
                If CType(messageTypeValue, String) <> GetType(T).FullName Then
                    Debug.WriteLine("Message type was {0} but expected type was {1}", CType(messageTypeValue, String), GetType(T).FullName)
                    Return False
                End If

                message = JsonHelper.FromJson(Of T)(messageBodyValue.ToString())
                Return True
            End If

            Return False
        End Function
    End Module
End Namespace
