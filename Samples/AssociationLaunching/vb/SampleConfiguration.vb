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
Imports Windows.ApplicationModel.Activation
Imports Windows.Foundation
Imports Windows.UI.ViewManagement
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Association Launching"

        Public ReadOnly Property Scenarios As New List(Of Scenario) From {
            New Scenario() With {.Title = "Launching a file", .ClassType = GetType(Scenario1_LaunchFile)},
            New Scenario() With {.Title = "Launching a URI", .ClassType = GetType(Scenario2_LaunchUri)},
            New Scenario() With {.Title = "Receiving a file", .ClassType = GetType(Scenario3_ReceiveFile)},
            New Scenario() With {.Title = "Receiving a URI", .ClassType = GetType(Scenario4_ReceiveUri)}}

        Public Shared ReadOnly Property ViewSizePreferences As ViewSizePreference() = {
            ViewSizePreference.Default,
            ViewSizePreference.UseLess,
            ViewSizePreference.UseHalf,
            ViewSizePreference.UseMore,
            ViewSizePreference.UseMinimum,
            ViewSizePreference.UseNone}

        Public Sub NavigateToPageWithParameter(pageIndex As Integer, parameter As Object)
            ScenarioControl.SelectedIndex = pageIndex
            ScenarioFrame.Navigate(Scenarios(pageIndex).ClassType, parameter)
        End Sub

        Public Shared Function GetElementLocation(e As Object) As Point
            Dim element As FrameworkElement = DirectCast(e, FrameworkElement)
            Dim buttonTransform As Windows.UI.Xaml.Media.GeneralTransform = element.TransformToVisual(Nothing)
            Dim desiredLocation As Point = buttonTransform.TransformPoint(New Point())
            desiredLocation.Y = desiredLocation.Y + element.ActualHeight
            Return desiredLocation
        End Function

    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type

        Public Overrides Function ToString() As String
            Return Title
        End Function
    End Class

    Partial Class App
        ' Handle file activations.
        ''' <summary>
        Protected Overrides Sub OnFileActivated(e As FileActivatedEventArgs)
            Dim rootFrame As Frame = CreateRootFrame()
            If rootFrame.Content Is Nothing Then
                If Not rootFrame.Navigate(GetType(MainPage)) Then
                    Throw New Exception("Failed to create initial page")
                End If
            End If

            Dim p = TryCast(rootFrame.Content, MainPage)
            p.NavigateToPageWithParameter(2, e)
            Window.Current.Activate()
        End Sub

        ' Handle protocol activations.
        ''' <summary>
        Protected Overrides Sub OnActivated(e As IActivatedEventArgs)
            If e.Kind = ActivationKind.Protocol Then
                Dim protocolArgs As ProtocolActivatedEventArgs = TryCast(e, ProtocolActivatedEventArgs)
                Dim rootFrame As Frame = CreateRootFrame()
                If rootFrame.Content Is Nothing Then
                    If Not rootFrame.Navigate(GetType(MainPage)) Then
                        Throw New Exception("Failed to create initial page")
                    End If
                End If

                Dim p = TryCast(rootFrame.Content, MainPage)
                p.NavigateToPageWithParameter(3, e)
                Window.Current.Activate()
            End If
        End Sub

    End Class
End Namespace
