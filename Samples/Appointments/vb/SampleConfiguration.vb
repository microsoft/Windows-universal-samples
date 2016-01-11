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
Imports Windows.Foundation
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Appointments"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Define an Appointment", .ClassType = GetType(Scenario1_Define)}, New Scenario() With {.Title = "Add an Appointment", .ClassType = GetType(Scenario2_Add)}, New Scenario() With {.Title = "Replace an Appointment", .ClassType = GetType(Scenario3_Replace)}, New Scenario() With {.Title = "Remove an Appointment", .ClassType = GetType(Scenario4_Remove)}, New Scenario() With {.Title = "Show Time Frame", .ClassType = GetType(Scenario5_Show)}, New Scenario() With {.Title = "Recurring Appointments", .ClassType = GetType(Scenario6_Recurrence)}}

        ' Helper function to calculate an element's rectangle in root-relative coordinates.
        Public Shared Function GetElementRect(element As Windows.UI.Xaml.FrameworkElement) As Rect
            Dim transform As Windows.UI.Xaml.Media.GeneralTransform = element.TransformToVisual(Nothing)
            Dim point As Point = transform.TransformPoint(New Point())
            Return New Rect(point, New Size(element.ActualWidth, element.ActualHeight))
        End Function
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace
