'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System.Collections.Generic
Imports System
Imports SDKTemplate

Namespace Global.SDKTemplate

    Public Partial Class MainPage

        ' Change the string below to reflect the name of your sample.
        ' This is used on the main page as the title of the sample.
        Public Const FEATURE_NAME As String = "Application Resources VB sample"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "String Resources In XAML", .ClassType = GetType(Scenario1)}, New Scenario() With {.Title = "File Resources In XAML", .ClassType = GetType(Scenario2)}, New Scenario() With {.Title = "String Resources In Code", .ClassType = GetType(Scenario3)}, New Scenario() With {.Title = "Resources in the AppX manifest", .ClassType = GetType(Scenario4)}, New Scenario() With {.Title = "Additional Resource Files", .ClassType = GetType(Scenario5)}, New Scenario() With {.Title = "Class Library Resources", .ClassType = GetType(Scenario6)}, New Scenario() With {.Title = "Runtime Changes/Events", .ClassType = GetType(Scenario7)}, New Scenario() With {.Title = "Application Languages", .ClassType = GetType(Scenario8)}, New Scenario() With {.Title = "Override Languages", .ClassType = GetType(Scenario9)}, New Scenario() With {.Title = "Multi-dimensional fallback", .ClassType = GetType(Scenario10)}, New Scenario() With {.Title = "Working with webservices", .ClassType = GetType(Scenario11)}, New Scenario() With {.Title = "Retrieving resources in non-UI threads", .ClassType = GetType(Scenario12)}, New Scenario() With {.Title = "File resources in code", .ClassType = GetType(Scenario13)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type

        Public Overrides Function ToString() As String
            Return Title
        End Function
    End Class
End Namespace
