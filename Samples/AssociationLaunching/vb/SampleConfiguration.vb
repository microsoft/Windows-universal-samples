'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
'
'*********************************************************
Imports System.Collections.Generic
Imports Windows.UI.ViewManagement
Imports Windows.UI.Xaml.Controls
Imports System
Imports AssociationLaunching

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        ' Change the string below to reflect the name of your sample.
        ' This is used on the main page as the title of the sample.
        Public Const FEATURE_NAME As String = "Association Launching"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Launching a file", .ClassType = GetType(LaunchFile)}, New Scenario() With {.Title = "Launching a URI", .ClassType = GetType(LaunchUri)}, New Scenario() With {.Title = "Receiving a file", .ClassType = GetType(ReceiveFile)}, New Scenario() With {.Title = "Receiving a URI", .ClassType = GetType(ReceiveUri)}}

        Public Shared ReadOnly Property ViewSizePreferences As ViewSizePreference() = {
            ViewSizePreference.Default,
            ViewSizePreference.UseLess,
            ViewSizePreference.UseHalf,
            ViewSizePreference.UseMore,
            ViewSizePreference.UseMinimum,
            ViewSizePreference.UseNone}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type

        Public Overrides Function ToString() As String
            Return Title
        End Function
    End Class
End Namespace
