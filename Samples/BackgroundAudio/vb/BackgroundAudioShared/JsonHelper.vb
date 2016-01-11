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
Imports System.IO
Imports System.Linq
Imports System.Runtime.Serialization
Imports System.Runtime.Serialization.Json
Imports System.Text
Imports System.Threading.Tasks

Namespace Global.BackgroundAudioShared

    ''' <summary>
    ''' Simple JSON serializer / deserializer for passing messages
    ''' between processes
    ''' </summary>
    Public Module JsonHelper

        ''' <summary>
        ''' Convert a serializable object to JSON
        ''' </summary>
        ''' <typeparam name="T">Type of object</typeparam>
        ''' <param name="data">Data model to convert to JSON</param>
        ''' <returns>JSON serialized string of data model</returns>
        Public Function ToJson(Of T)(data As T) As String
            Dim serializer = New DataContractJsonSerializer(GetType(T))
            Using ms As MemoryStream = New MemoryStream()
                serializer.WriteObject(ms, data)
                Dim jsonArray = ms.ToArray()
                Return Encoding.UTF8.GetString(jsonArray, 0, jsonArray.Length)
            End Using
        End Function

        ''' <summary>
        ''' Convert from JSON to a serializable object
        ''' </summary>
        ''' <typeparam name="T">Type to convert to</typeparam>
        ''' <param name="json">JSON serialized object to convert from</param>
        ''' <returns>Object deserialized from JSON</returns>
        Public Function FromJson(Of T)(json As String) As T
            Dim deserializer = New DataContractJsonSerializer(GetType(T))
            Try
                Using ms As MemoryStream = New MemoryStream(Encoding.UTF8.GetBytes(json))
                    Return CType(deserializer.ReadObject(ms), T)
                End Using
            Catch ex As SerializationException
                Throw New SerializationException("Unable to deserialize JSON: " & json, ex)
            End Try
        End Function
    End Module
End Namespace
