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

    Public Class VideoMetaData

        Public Property Id As String

        Public Property Title As String

        Public Property PubDate As DateTime

        Public Property License As String

        Public Property WebPageLink As Uri

        Public Property VideoLink As Uri

        Public Property Thumbnail As Uri
    End Class
End Namespace
