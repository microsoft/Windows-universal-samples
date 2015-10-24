'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate
Imports System
Imports Windows.Foundation
Imports Windows.UI.Core
Imports Windows.Storage

Namespace Global.ApplicationDataSample

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class HighPriority
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim roamingSettings As ApplicationDataContainer = Nothing

        Dim applicationData As ApplicationData = Nothing

        Dim _dataChangedHandler As TypedEventHandler(Of ApplicationData, Object) = Nothing

        Public Sub New()
            Me.InitializeComponent()
            applicationData = ApplicationData.Current
            roamingSettings = applicationData.RoamingSettings
            DisplayOutput(False)
        End Sub

        Sub IncrementHighPriority_Click(sender As [Object], e As RoutedEventArgs)
            Dim counter As Integer = Convert.ToInt32(roamingSettings.Values("HighPriority"))
            roamingSettings.Values("HighPriority") = counter + 1
            DisplayOutput(False)
        End Sub

        Async Sub DataChangedHandler(appData As Windows.Storage.ApplicationData, o As Object)
            Await Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                DisplayOutput(True)
            End Sub)
        End Sub

        Sub DisplayOutput(remoteUpdate As Boolean)
            Dim counter As Integer = Convert.ToInt32(roamingSettings.Values("HighPriority"))
            OutputTextBlock.Text = "Counter: " & counter & (If(remoteUpdate, " (updated remotely)", ""))
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            _dataChangedHandler = New TypedEventHandler(Of ApplicationData, Object)(AddressOf dataChangedHandler)
            AddHandler applicationData.DataChanged, AddressOf dataChangedHandler
        End Sub

        ''' <summary>
        ''' Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        ''' </summary>
        ''' <param name="e">
        ''' Event data that can be examined by overriding code. The event data is representative
        ''' of the navigation that will unload the current Page unless canceled. The
        ''' navigation can potentially be canceled by setting Cancel.
        ''' </param>
        Protected Overrides Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            MyBase.OnNavigatingFrom(e)
            RemoveHandler applicationData.DataChanged, AddressOf dataChangedHandler
            _dataChangedHandler = Nothing
        End Sub
    End Class
End Namespace
