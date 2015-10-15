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
Imports System.Text
Imports Windows.Globalization

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario8
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            UpdateCurrentAppLanguageMessage()
            AddHandler LanguageOverrideCombo.LanguageOverrideChanged, AddressOf LanguageOverrideCombo_LanguageOrverrideChanged
        End Sub

        Sub LanguageOverrideCombo_LanguageOrverrideChanged(sender As Object, e As EventArgs)
            UpdateCurrentAppLanguageMessage()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Scenario8Button_Show' button.  You would replace this with your own handler
        ''' if you have a button or buttons on this page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Scenario8Button_Show_Click(sender As Object, e As RoutedEventArgs)
            Dim b As Button = TryCast(sender, Button)
            If b IsNot Nothing Then
                Dim defaultContextForCurrentView As ResourceContext = ResourceContext.GetForCurrentView()
                Dim stringResourcesResourceMap As ResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources")
                Scenario8MessageTextBlock.Text = stringResourcesResourceMap.GetValue("string1", defaultContextForCurrentView).ValueAsString
            End If
        End Sub

        Private Sub UpdateCurrentAppLanguageMessage()
            Me.Scenario8AppLanguagesTextBlock.Text = "Current app language(s): " & GetAppLanguagesAsFormattedString()
        End Sub

        Private Function GetAppLanguagesAsFormattedString() As String
            Dim countLanguages = ApplicationLanguages.Languages.Count
            Dim sb As StringBuilder = New StringBuilder()
            For i = 0 To countLanguages - 1 - 1
                sb.Append(ApplicationLanguages.Languages(i))
                sb.Append(", ")
            Next

            sb.Append(ApplicationLanguages.Languages(countLanguages - 1))
            Return sb.ToString()
        End Function
    End Class
End Namespace
