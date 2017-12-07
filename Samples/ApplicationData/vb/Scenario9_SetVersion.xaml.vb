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
    Public NotInheritable Partial Class SetVersion
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim appData As ApplicationData = Nothing

        Const settingName As String = "SetVersionSetting"

        Const settingValue0 As String = "Data.v0"

        Const settingValue1 As String = "Data.v1"

        Public Sub New()
            Me.InitializeComponent()
            appData = ApplicationData.Current
            DisplayOutput()
        End Sub

        Sub SetVersionHandler0(request As SetVersionRequest)
            Dim deferral As SetVersionDeferral = request.GetDeferral()
            Dim version As UInteger = appData.Version
            Select version
                Case 0
                     Case 1
                    appData.LocalSettings.Values(settingName) = settingValue0
                     Case Else
                    Throw New Exception("Unexpected ApplicationData Version: " & version)
            End Select

            deferral.Complete()
        End Sub

        Sub SetVersionHandler1(request As SetVersionRequest)
            Dim deferral As SetVersionDeferral = request.GetDeferral()
            Dim version As UInteger = appData.Version
            Select version
                Case 0
                    appData.LocalSettings.Values(settingName) = settingValue1
                     Case 1
                     Case Else
                    Throw New Exception("Unexpected ApplicationData Version: " & version)
            End Select

            deferral.Complete()
        End Sub

        Async Sub SetVersion0_Click(sender As [Object], e As RoutedEventArgs)
            Await appData.SetVersionAsync(0, New ApplicationDataSetVersionHandler(AddressOf SetVersionHandler0))
            DisplayOutput()
        End Sub

        Async Sub SetVersion1_Click(sender As [Object], e As RoutedEventArgs)
            Await appData.SetVersionAsync(1, New ApplicationDataSetVersionHandler(AddressOf SetVersionHandler1))
            DisplayOutput()
        End Sub

        Sub DisplayOutput()
            OutputTextBlock.Text = "Version: " & appData.Version
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
