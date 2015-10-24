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
    Public NotInheritable Partial Class Scenario9
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

        ''' <summary>
        ''' Invoked when the user has selected a different scenario and navigation away from
        ''' this page is about to occur.
        ''' </summary>
        ''' <param name="e">Event data that describes the navigation that has unloaded this
        ''' page.</param>
        Protected Overrides Sub OnNavigatedFrom(e As NavigationEventArgs)
            ResourceContext.GetForCurrentView().Reset()
            MyBase.OnNavigatedFrom(e)
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Scenario9Button_Show' button.  You would replace this with your own handler
        ''' if you have a button or buttons on this page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Scenario9Button_Show_Click(sender As Object, e As RoutedEventArgs)
            Dim b As Button = TryCast(sender, Button)
            If b IsNot Nothing Then
                ' A langauge override will be set on the default context for the current view.
                ' When navigating between different scenarios in this sample, the content for each
                ' scenario is loaded into a host page that remains constant. The view (meaning, the 
                ' CoreWindow) remains the same, and so it is the same default context that's in use. 
                ' Thus, an override set here can impact behavior in other scenarios.
                '
                ' However, the description for the scenario may give the impression that a value 
                ' being set is local to this scenario. Also, when entering this scenario, the combo 
                ' box always gets set to the first item, which can add to the confusion. To avoid 
                ' confusion when using this sample, the override that gets set here will be cleared 
                ' when the user navigates away from this scenario (in the OnNavigatedFrom event 
                ' handler). In a real app, whether and when to clear an override will depend on
                ' the desired behaviour and the design of the app.
                Dim context = ResourceContext.GetForCurrentView()
                Dim selectedLanguage = Scenario9ComboBox.SelectedValue
                If selectedLanguage IsNot Nothing Then
                    Dim lang = New List(Of String)()
                    lang.Add(selectedLanguage.ToString())
                    context.Languages = lang
                    Dim resourceStringMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources")
                    Me.Scenario9TextBlock.Text = resourceStringMap.GetValue("string1", context).ValueAsString
                End If
            End If
        End Sub
    End Class
End Namespace
