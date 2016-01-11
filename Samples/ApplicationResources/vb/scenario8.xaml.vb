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

        Dim lastSelectedIndex As Integer

        Public Sub New()
            Me.InitializeComponent()
            PopulateComboBox()
            UpdateCurrentAppLanguageMessage()
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Scenario8Button_Show' button.  You would replace this with your own handler
        ''' if you have a button or buttons on this page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ShowText()
            Dim defaultContextForCurrentView As ResourceContext = ResourceContext.GetForCurrentView()
            Dim stringResourcesResourceMap As ResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources")
            Scenario8MessageTextBlock.Text = stringResourcesResourceMap.GetValue("string1", defaultContextForCurrentView).ValueAsString
        End Sub

        Private Sub LanguageOverrideCombo_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            Dim combo As ComboBox = TryCast(sender, ComboBox)
            Dim item As ComboBoxItem = TryCast(combo.SelectedValue, ComboBoxItem)
            Dim languageTag As String = TryCast(item.Tag, String)

            ' Ignore the divider (tag = "-")
            If languageTag = "-" Then
                combo.SelectedIndex = lastSelectedIndex
            Else
                lastSelectedIndex = combo.SelectedIndex
                ' Set the persistent language override
                Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride = languageTag
                ' update current app languages message
                UpdateCurrentAppLanguageMessage()
            End If
        End Sub

        Private Sub AddItemForLanguageTag(languageTag As String)
            Dim language As Language = New Language(languageTag)
            Dim item As ComboBoxItem = New ComboBoxItem With {.Content = language.DisplayName, .Tag = languageTag}
            LanguageOverrideCombo.Items.Add(item)

            ' Select this item if it is the primary language override.
            If languageTag = ApplicationLanguages.PrimaryLanguageOverride Then
                LanguageOverrideCombo.SelectedItem = item
            End If
        End Sub

        Private Sub PopulateComboBox()
            ' First show the default setting
            LanguageOverrideCombo.Items.Add(New ComboBoxItem With {.Content = "Use language preferences (recommended)", .Tag = ""})
            LanguageOverrideCombo.SelectedIndex = 0

            ' If there are app languages that the user speaks, show them next

            ' Note: the first (non-override) language, if set as the primary language override
            ' would give the same result as not having any primary language override. There's
            ' still a difference, though: If the user changes their language preferences, the 
            ' default setting (no override) would mean that the actual primary app language
            ' could change. But if it's set as an override, then it will remain the primary
            ' app language after the user changes their language preferences.

            For Each languageTag In ApplicationLanguages.Languages
                AddItemForLanguageTag(languageTag)
            Next

            ' Now add a divider followed by all the app manifest languages (in case the user
            ' wants to pick a language not currently in their profile)

            ' NOTE: If an app is deployed using a bundle with resource packages, the following
            ' addition to the list may not be useful: The set of languages returned by 
            ' ApplicationLanguages.ManifestLanguages will consist of only those manifest 
            ' languages in the main app package or in the resource packages that are installed 
            ' and registered for the current user. Language resource packages get deployed for 
            ' the user if the language is in the user's profile. Therefore, the only difference 
            ' from the set returned by ApplicationLanguages.Languages above would depend on 
            ' which languages are included in the main app package.

            LanguageOverrideCombo.Items.Add(New ComboBoxItem With {.Content = "——————", .Tag = "-"})

            ' In a production app, this list should be sorted, but that's beyond the
            ' focus of this sample.
            For Each languageTag In ApplicationLanguages.ManifestLanguages
                AddItemForLanguageTag(languageTag)
            Next

            AddHandler LanguageOverrideCombo.SelectionChanged, AddressOf LanguageOverrideCombo_SelectionChanged

            lastSelectedIndex = LanguageOverrideCombo.SelectedIndex

        End Sub

        Private Sub UpdateCurrentAppLanguageMessage()
            Me.Scenario8AppLanguagesTextBlock.Text = "Current app language(s): " & GetAppLanguagesAsFormattedString()
        End Sub

        Private Function GetAppLanguagesAsFormattedString() As String
            Return String.Join(", ", ApplicationLanguages.Languages)
        End Function
    End Class
End Namespace
