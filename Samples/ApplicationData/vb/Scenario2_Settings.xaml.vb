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
    Public NotInheritable Partial Class Settings
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim roamingSettings As ApplicationDataContainer = Nothing

        Const SettingName As String = "exampleSetting"

        Public Sub New()
            Me.InitializeComponent()
            roamingSettings = ApplicationData.Current.RoamingSettings
            DisplayOutput()
        End Sub

        Sub WriteSetting_Click(sender As [Object], e As RoutedEventArgs)
            roamingSettings.Values(SettingName) = "Hello World"
            DisplayOutput()
        End Sub

        Sub DeleteSetting_Click(sender As [Object], e As RoutedEventArgs)
            roamingSettings.Values.Remove(SettingName)
            DisplayOutput()
        End Sub

        Sub DisplayOutput()
            Dim value As [Object] = roamingSettings.Values(SettingName)
            OutputTextBlock.Text = String.Format("Setting: {0}", (If(value Is Nothing, "<empty>", ("""" & value & """"))))
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
