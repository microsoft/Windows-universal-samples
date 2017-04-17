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
Imports Windows.Storage

Namespace Global.ApplicationDataSample

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class CompositeSettings
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim roamingSettings As ApplicationDataContainer = Nothing

        Const settingName As String = "exampleCompositeSetting"

        Const settingName1 As String = "one"

        Const settingName2 As String = "hello"

        Public Sub New()
            Me.InitializeComponent()
            roamingSettings = ApplicationData.Current.RoamingSettings
            DisplayOutput()
        End Sub

        Sub WriteCompositeSetting_Click(sender As [Object], e As RoutedEventArgs)
            Dim composite As ApplicationDataCompositeValue = New ApplicationDataCompositeValue()
            composite(settingName1) = 1
            composite(settingName2) = "world"
            roamingSettings.Values(settingName) = composite
            DisplayOutput()
        End Sub

        Sub DeleteCompositeSetting_Click(sender As [Object], e As RoutedEventArgs)
            roamingSettings.Values.Remove(settingName)
            DisplayOutput()
        End Sub

        Sub DisplayOutput()
            Dim composite As ApplicationDataCompositeValue = CType(roamingSettings.Values(settingName), ApplicationDataCompositeValue)
            Dim output As String
            If composite Is Nothing Then
                output = "Composite Setting: <empty>"
            Else
                output = String.Format("Composite Setting: {{{0} = {1}, {2} = ""{3}""}}", settingName1, composite(settingName1), settingName2, composite(settingName2))
            End If

            OutputTextBlock.Text = output
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
        End Sub
    End Class
End Namespace
