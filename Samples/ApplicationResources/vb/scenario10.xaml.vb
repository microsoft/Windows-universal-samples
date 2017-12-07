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
    Public NotInheritable Partial Class Scenario10
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
        ''' This is the click handler for the 'Scenario10Button_Show' button.  You would replace this with your own handler
        ''' if you have a button or buttons on this page.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Scenario10Button_Show_Click(sender As Object, e As RoutedEventArgs)
            Dim b As Button = TryCast(sender, Button)
            If b IsNot Nothing Then
                ' use a cloned context for this scenario so that qualifier values set
                ' in this scenario don't impact behaviour in other scenarios that
                ' use a default context for the view (crossover effects between
                ' the scenarios will not be expected)
                Dim context = ResourceContext.GetForCurrentView().Clone()
                Dim selectedLanguage = Scenario10ComboBox_Language.SelectedValue
                Dim selectedScale = Scenario10ComboBox_Scale.SelectedValue
                Dim selectedContrast = Scenario10ComboBox_Contrast.SelectedValue
                Dim selectedHomeRegion = Scenario10ComboBox_HomeRegion.SelectedValue
                If selectedLanguage IsNot Nothing Then
                    context.QualifierValues("language") = selectedLanguage.ToString()
                End If

                If selectedScale IsNot Nothing Then
                    context.QualifierValues("scale") = selectedScale.ToString()
                End If

                If selectedContrast IsNot Nothing Then
                    context.QualifierValues("contrast") = selectedContrast.ToString()
                End If

                If selectedHomeRegion IsNot Nothing Then
                    context.QualifierValues("homeregion") = selectedHomeRegion.ToString()
                End If

                Scenario10_SearchMultipleResourceIds(context, New String() {"LanguageOnly", "ScaleOnly", "ContrastOnly", "HomeRegionOnly", "MultiDimensional"})
            End If
        End Sub

        Sub Scenario10_SearchMultipleResourceIds(context As ResourceContext, resourceIds As String())
            Me.Scenario10TextBlock.Text = ""
            Dim dimensionMap = ResourceManager.Current.MainResourceMap.GetSubtree("dimensions")
            For Each id In resourceIds
                Dim namedResource As NamedResource = Nothing
                If dimensionMap.TryGetValue(id, namedResource) Then
                    Dim resourceCandidates = namedResource.ResolveAll(context)
                    Scenario10_ShowCandidates(id, resourceCandidates)
                End If
            Next
        End Sub

        Sub Scenario10_ShowCandidates(resourceId As String, resourceCandidates As IReadOnlyList(Of ResourceCandidate))
            ' print 'resourceId', 'found value', 'qualifer info', 'matching condition'
            Dim outText As String = "resourceId: dimensions\" & resourceId & vbCrLf
            Dim i As Integer = 0
            For Each candidate In resourceCandidates
                Dim value = candidate.ValueAsString
                outText &= "    Candidate " & i.ToString() & ":" & value & vbCrLf
                Dim j As Integer = 0
                For Each qualifier In candidate.Qualifiers
                    Dim qualifierName = qualifier.QualifierName
                    Dim qualifierValue = qualifier.QualifierValue
                    outText &= "        Qualifer: " & qualifierName & ": " & qualifierValue & vbCrLf
                    outText &= "        Matching: IsMatch (" & qualifier.IsMatch.ToString() & ")  " & "IsDefault (" & qualifier.IsDefault.ToString() & ")" & vbCrLf
                    j = j + 1
                Next

                i = i + 1
            Next

            Me.Scenario10TextBlock.Text &= outText & vbCrLf
        End Sub
    End Class
End Namespace
