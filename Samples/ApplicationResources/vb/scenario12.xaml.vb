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
Imports Windows.ApplicationModel.Resources.Core
Imports System.Collections.Generic

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario12
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
        End Sub

        Private Async Sub Scenario12Button_Show_Click(sender As Object, e As RoutedEventArgs)
            ' Two coding patterns will be used:
            '   1. Get a ResourceContext on the UI thread using GetForCurrentView and pass 
            '      to the non-UI thread
            '   2. Get a ResourceContext on the non-UI thread using GetForViewIndependentUse
            '
            ' Two analogous patterns could be used for ResourceLoader instead of ResourceContext.
            ' pattern 1: get a ResourceContext for the UI thread
            Dim defaultContextForUiThread As ResourceContext = ResourceContext.GetForCurrentView()
            ' pattern 2: we'll create a view-independent context in the non-UI worker thread
            ' We need some things in order to display results in the UI (doing that
            ' for purposes of this sample, to show that work was actually done in the
            ' worker thread):
            Dim uiDependentResourceList As List(Of String) = New List(Of String)()
            Dim uiIndependentResourceList As List(Of String) = New List(Of String)()
            Await Windows.System.Threading.ThreadPool.RunAsync(Sub(source)
                Dim stringResourceMap As ResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources")
                ' pattern 1: the defaultContextForUiThread variable was created above and is visible here
                ' pattern 2: get a view-independent ResourceContext
                Dim defaultViewIndependentResourceContext As ResourceContext = ResourceContext.GetForViewIndependentUse()
                For i = 0 To 4 - 1
                    ' pattern 1: use the ResourceContext from the UI thread
                    Dim listItem1 As String = stringResourceMap.GetValue("string1", defaultContextForUiThread).ValueAsString
                    uiDependentResourceList.Add(listItem1)
                    ' pattern 2: use the view-independent ResourceContext
                    Dim listItem2 As String = stringResourceMap.GetValue("string1", defaultViewIndependentResourceContext).ValueAsString
                    uiIndependentResourceList.Add(listItem2)
                Next
            End Sub)
            ViewDependentResourcesList.ItemsSource = uiDependentResourceList
            ViewIndependentResourcesList.ItemsSource = uiIndependentResourceList
        End Sub
    End Class
End Namespace
