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
Imports ScreenCasting.Util
Imports System
Imports System.Collections.Generic
Imports Windows.ApplicationModel.Activation
Imports Windows.UI.ViewManagement
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class MainPage
        Inherits Page

        Public Shared Current As MainPage

        Public ProjectionViewPageControl As ViewLifetimeControl

        Public Sub New()
            LoadScenarios()
            Me.DataContext = Me
            Me.InitializeComponent()
            Current = Me
            SampleTitle.Text = FEATURE_NAME
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            ApplicationView.GetForCurrentView().ExitFullScreenMode()
            AddHandler Me.ScenarioControl.SelectionChanged, AddressOf ScenarioControl_SelectionChanged
            Try
                ScenarioControl.ItemsSource = Scenarios
                If TypeOf e.Parameter Is DialReceiverActivatedEventArgs Then
                    NavigateToScenario(GetType(Scenario04), e.Parameter)
                Else
                    ScenarioControl.SelectedIndex = 0
                End If
            Catch ex As Exception
                Window.Current.Content = New Frame()
                Dim rootFrame As Frame = TryCast(Window.Current.Content, Frame)
                rootFrame.Navigate(GetType(UnhandledExceptionPage), Nothing)
                Dim content = CType(rootFrame.Content, UnhandledExceptionPage)
                content.StatusMessage = ex.Message & ex.StackTrace
            End Try
        End Sub

        ''' <summary>
        ''' Called whenever the user changes selection in the scenarios list.  This method will navigate to the respective
        ''' sample scenario page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioControl_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            NotifyUser(String.Empty, NotifyType.StatusMessage)
            Dim scenarioListBox As ListBox = TryCast(sender, ListBox)
            Dim s As Scenario = Scenarios(scenarioListBox.SelectedIndex)
            If s IsNot Nothing Then
                ScenarioFrame.Navigate(s.ClassType)
                If Window.Current.Bounds.Width < 640 Then
                    Splitter.IsPaneOpen = False
                    StatusBorder.Visibility = Visibility.Collapsed
                Else
                    Splitter.IsPaneOpen = True
                    StatusBorder.Visibility = Visibility.Visible
                End If
            End If
        End Sub

        Friend Sub NavigateToScenario(scenarioType As Type, args As Object)
            Try
                NotifyUser(String.Empty, NotifyType.StatusMessage)
                Dim scenarioListBox As ListBox = Me.ScenarioControl
                Dim selectedIndex As Integer = -1
                RemoveHandler Me.ScenarioControl.SelectionChanged, AddressOf ScenarioControl_SelectionChanged
                For idx = 0 To scenarioListBox.Items.Count - 1
                    If(CType(scenarioListBox.Items(idx), Scenario)).ClassType Is scenarioType Then
                        selectedIndex = idx
                        Exit For
                    End If
                Next

                If selectedIndex > -1 Then
                    scenarioListBox.SelectedIndex = selectedIndex
                End If

                Dim s As Scenario = TryCast(scenarioListBox.SelectedItem, Scenario)
                If s IsNot Nothing Then
                    ScenarioFrame.Navigate(s.ClassType, args)
                    If Window.Current.Bounds.Width < 640 Then
                        Splitter.IsPaneOpen = False
                        StatusBorder.Visibility = Visibility.Collapsed
                    Else
                        Splitter.IsPaneOpen = True
                        StatusBorder.Visibility = Visibility.Visible
                    End If
                End If

                AddHandler Me.ScenarioControl.SelectionChanged, AddressOf ScenarioControl_SelectionChanged
            Catch ex As Exception
                Dim rootFrame As Frame = TryCast(Window.Current.Content, Frame)
                Window.Current.Content = New Frame()
                rootFrame.Navigate(GetType(UnhandledExceptionPage), ex.Message)
                Dim content = CType(rootFrame.Content, UnhandledExceptionPage)
                content.StatusMessage = ex.Message & ex.StackTrace
            End Try
        End Sub

        ''' <summary>
        ''' Used to display messages to the user
        ''' </summary>
        ''' <param name="strMessage"></param>
        ''' <param name="type"></param>
        Public Sub NotifyUser(strMessage As String, type As NotifyType)
            Select type
                Case NotifyType.StatusMessage
                    StatusBorder.Background = New SolidColorBrush(Windows.UI.Colors.Green)
                     Case NotifyType.ErrorMessage
                    StatusBorder.Background = New SolidColorBrush(Windows.UI.Colors.Red)
                     End Select

            StatusBlock.Text = strMessage
            StatusBorder.Visibility = If((StatusBlock.Text <> String.Empty), Visibility.Visible, Visibility.Collapsed)
        End Sub

        Async Sub Footer_Click(sender As Object, e As RoutedEventArgs)
            Await Windows.System.Launcher.LaunchUriAsync(New Uri((CType(sender, HyperlinkButton)).Tag.ToString()))
        End Sub

        Private Sub Button_Click(sender As Object, e As RoutedEventArgs)
            Splitter.IsPaneOpen = If((Splitter.IsPaneOpen = True), False, True)
            StatusBorder.Visibility = Visibility.Collapsed
        End Sub
    End Class

    Public Enum NotifyType
        StatusMessage
        ErrorMessage
    End Enum
End Namespace
