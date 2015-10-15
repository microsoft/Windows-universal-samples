Imports System
Imports System.Collections.Generic
Imports System.IO
Imports System.Linq
Imports Windows.Foundation
Imports Windows.Foundation.Collections
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation
Imports Windows.Globalization

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class LanguageOverride
        Inherits UserControl

        Dim comboBoxValues As List(Of ComboBoxValue)

        Dim lastSelectionIndex As Integer

        Public Delegate Sub LanguageOverrideChangedEventHandler(sender As Object, e As EventArgs)

        Public Event LanguageOverrideChanged As LanguageOverrideChangedEventHandler

        Public Sub New()
            Me.InitializeComponent()
            AddHandler Loaded, AddressOf Control_Loaded
        End Sub

        Sub Control_Loaded(sender As Object, e As RoutedEventArgs)
            comboBoxValues = New List(Of ComboBoxValue)()
            comboBoxValues.Add(New ComboBoxValue() With {.DisplayName = "Use language preferences (recommended)", .LanguageTag = ""})
            For i = 0 To ApplicationLanguages.Languages.Count - 1
                Me.LanguageOverrideComboBox_AddLanguage(New Windows.Globalization.Language(ApplicationLanguages.Languages(i)))
            Next

            comboBoxValues.Add(New ComboBoxValue() With {.DisplayName = "——————", .LanguageTag = "-"})
            ' Create a List and sort it before adding items
            Dim manifestLanguageObjects As List(Of Windows.Globalization.Language) = New List(Of Windows.Globalization.Language)()
            For Each lang In ApplicationLanguages.ManifestLanguages
                manifestLanguageObjects.Add(New Windows.Globalization.Language(lang))
            Next

            Dim orderedManifestLanguageObjects As IEnumerable(Of Windows.Globalization.Language) = manifestLanguageObjects.OrderBy(Function(lang) lang.DisplayName)
            For Each lang In orderedManifestLanguageObjects
                Me.LanguageOverrideComboBox_AddLanguage(lang)
            Next

            LanguageOverrideComboBox.ItemsSource = comboBoxValues
            LanguageOverrideComboBox.SelectedIndex = comboBoxValues.FindIndex(AddressOf FindCurrent)
            AddHandler LanguageOverrideComboBox.SelectionChanged, AddressOf LanguageOverrideComboBox_SelectionChanged
            lastSelectionIndex = LanguageOverrideComboBox.SelectedIndex
        End Sub

        Private Sub LanguageOverrideComboBox_AddLanguage(lang As Windows.Globalization.Language)
            comboBoxValues.Add(New ComboBoxValue() With {.DisplayName = lang.NativeName, .LanguageTag = lang.LanguageTag})
        End Sub

        Private Shared Function FindCurrent(value As ComboBoxValue) As Boolean
            If value.LanguageTag = Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride Then
                Return True
            End If

            Return False
        End Function

        Private Sub LanguageOverrideComboBox_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            Dim combo As ComboBox = TryCast(sender, ComboBox)
            If combo.SelectedValue.ToString() = "-" Then
                combo.SelectedIndex = lastSelectionIndex
            Else
                lastSelectionIndex = combo.SelectedIndex
                Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride = combo.SelectedValue.ToString()
                RaiseEvent LanguageOverrideChanged(Me, New EventArgs())
            End If
        End Sub
    End Class

    Public Class ComboBoxValue

        Public Property DisplayName As String

        Public Property LanguageTag As String
    End Class
End Namespace
