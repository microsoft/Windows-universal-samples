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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class MainPage
        Inherits Page

        Public Shared Current As MainPage

        Public Sub New()
            Me.InitializeComponent()
            Current = Me
            SampleTitle.Text = FEATURE_NAME
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            ScenarioControl.ItemsSource = scenarios
            If Window.Current.Bounds.Width < 640 Then
                ScenarioControl.SelectedIndex = -1
            Else
                ScenarioControl.SelectedIndex = 0
            End If
        End Sub

        ''' <summary>
        ''' Called whenever the user changes selection in the scenarios list.  This method will navigate to the respective
        ''' sample scenario page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioControl_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            NotifyUser([String].Empty, NotifyType.StatusMessage)
            Dim scenarioListBox As ListBox = TryCast(sender, ListBox)
            Dim s As Scenario = TryCast(scenarioListBox.SelectedItem, Scenario)
            If s IsNot Nothing Then
                ScenarioFrame.Navigate(s.ClassType)
                If Window.Current.Bounds.Width < 640 Then
                    Splitter.IsPaneOpen = False
                End If
            End If
        End Sub

        ''' <summary>
        ''' Used to display messages to the user
        ''' </summary>
        ''' <param name="strMessage"></param>
        ''' <param name="type"></param>
        Public Sub NotifyUser(strMessage As String, type As NotifyType)
            Select Case type
                Case NotifyType.StatusMessage
                    StatusBorder.Background = New SolidColorBrush(Windows.UI.Colors.Green)
                Case NotifyType.ErrorMessage
                    StatusBorder.Background = New SolidColorBrush(Windows.UI.Colors.Red)
            End Select

            StatusBlock.Text = strMessage
            StatusBorder.Visibility = If((StatusBlock.Text <> [String].Empty), Visibility.Visible, Visibility.Collapsed)
            If StatusBlock.Text <> [String].Empty Then
                StatusBorder.Visibility = Visibility.Visible
                StatusPanel.Visibility = Visibility.Visible
            Else
                StatusBorder.Visibility = Visibility.Collapsed
                StatusPanel.Visibility = Visibility.Collapsed
            End If
        End Sub

        Async Sub Footer_Click(sender As Object, e As RoutedEventArgs)
            Await Windows.System.Launcher.LaunchUriAsync(New Uri((CType(sender, HyperlinkButton)).Tag.ToString()))
        End Sub

        Private Sub Button_Click(sender As Object, e As RoutedEventArgs)
            Splitter.IsPaneOpen = Not Splitter.IsPaneOpen
        End Sub
    End Class

    Public Enum NotifyType
        StatusMessage
        ErrorMessage
    End Enum

    Public Class ScenarioBindingConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As Type, parameter As Object, language As String) As Object Implements IValueConverter.Convert
            Dim s As Scenario = TryCast(value, Scenario)
            Return (MainPage.Current.scenarios.IndexOf(s) + 1) & ") " & s.Title
        End Function

        Public Function ConvertBack(value As Object, targetType As Type, parameter As Object, language As String) As Object Implements IValueConverter.ConvertBack
            Return True
        End Function
    End Class
End Namespace
