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
    Public NotInheritable Partial Class SettingContainer
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim localSettings As ApplicationDataContainer = Nothing

        Const containerName As String = "exampleContainer"

        Const settingName As String = "exampleSetting"

        Public Sub New()
            Me.InitializeComponent()
            localSettings = ApplicationData.Current.LocalSettings
            DisplayOutput()
        End Sub

        Sub CreateContainer_Click(sender As [Object], e As RoutedEventArgs)
            Dim container As ApplicationDataContainer = localSettings.CreateContainer(containerName, ApplicationDataCreateDisposition.Always)
            DisplayOutput()
        End Sub

        Sub DeleteContainer_Click(sender As [Object], e As RoutedEventArgs)
            localSettings.DeleteContainer(containerName)
            DisplayOutput()
        End Sub

        Sub WriteSetting_Click(sender As [Object], e As RoutedEventArgs)
            If localSettings.Containers.ContainsKey(containerName) Then
                localSettings.Containers(containerName).Values(settingName) = "Hello World"
            End If

            DisplayOutput()
        End Sub

        Sub DeleteSetting_Click(sender As [Object], e As RoutedEventArgs)
            If localSettings.Containers.ContainsKey(containerName) Then
                localSettings.Containers(containerName).Values.Remove(settingName)
            End If

            DisplayOutput()
        End Sub

        Sub DisplayOutput()
            Dim hasContainer As Boolean = localSettings.Containers.ContainsKey(containerName)
            Dim hasSetting As Boolean = If(hasContainer, localSettings.Containers(containerName).Values.ContainsKey(settingName), False)
            Dim output As String = String.Format("Container Exists: {0}" & vbLf & "Setting Exists: {1}", If(hasContainer, "true", "false"), If(hasSetting, "true", "false"))
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
