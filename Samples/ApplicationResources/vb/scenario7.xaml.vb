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

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario7
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim defaultContextForCurrentView As ResourceContext

        Public Sub New()
            Me.InitializeComponent()
            defaultContextForCurrentView = ResourceContext.GetForCurrentView()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            Me.Scenario7TextBlock.Text = ""
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Scenario7Button_Show' button.  You would replace this with your own handler
        ''' if you have a button or buttons on this page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Scenario7Button_Show_Click(sender As Object, e As RoutedEventArgs)
            Dim b As Button = TryCast(sender, Button)
            If b IsNot Nothing Then
                Me.Scenario7TextBlock.Text = ResourceManager.Current.MainResourceMap.GetValue("Resources/string1", defaultContextForCurrentView).ValueAsString
                AddHandler defaultContextForCurrentView.QualifierValues.MapChanged, Async Sub(s, m)
                    Await Me.Scenario7TextBlock.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                        Me.Scenario7TextBlock.Text = ResourceManager.Current.MainResourceMap.GetValue("Resources/string1", defaultContextForCurrentView).ValueAsString
                    End Sub)
                End Sub
            End If
        End Sub
    End Class
End Namespace
