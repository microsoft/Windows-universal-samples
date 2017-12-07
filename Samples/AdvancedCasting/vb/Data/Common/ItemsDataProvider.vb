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

Namespace Global.ScreenCasting.Data.Common

    Public MustInherit Class ItemsDataProvider

        Public MustOverride Function GetAll(max_results As Integer) As List(Of VideoMetaData)

        Public MustOverride Function GetRandomVideo() As VideoMetaData

        Public MustOverride Function GetFromID(ID As String) As VideoMetaData
    End Class
End Namespace
