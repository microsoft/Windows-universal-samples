'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports SDKTemplate
Imports System
Imports Windows.ApplicationModel.AppService
Imports Windows.Foundation.Collections
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.AppServicesClient

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class OpenCloseConnectionScenario
        Inherits Page

        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Private Async Sub GenerateRandomNumber_Click(sender As Object, e As RoutedEventArgs)
            'Parse user input
            Dim minValueInput As Integer = 0
            Dim valueParsed As Boolean = Integer.TryParse(MinValue.Text, minValueInput)
            If Not valueParsed Then
                rootPage.NotifyUser("The Minimum Value should be a valid integer", NotifyType.ErrorMessage)
                Return
            End If

            Dim maxValueInput As Integer = 0
            valueParsed = Integer.TryParse(MaxValue.Text, maxValueInput)
            If Not valueParsed Then
                rootPage.NotifyUser("The Maximum Value should be a valid integer", NotifyType.ErrorMessage)
                Return
            End If

            If maxValueInput <= minValueInput Then
                rootPage.NotifyUser("Maximum Value must be larger than Minimum Value", NotifyType.ErrorMessage)
                Return
            End If

            Using connection = New AppServiceConnection()
                connection.AppServiceName = "com.microsoft.randomnumbergenerator"
                connection.PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CS_8wekyb3d8bbwe"
                Dim status As AppServiceConnectionStatus = Await connection.OpenAsync()
                If status = AppServiceConnectionStatus.Success Then
                    rootPage.NotifyUser("Connection established", NotifyType.StatusMessage)
                End If

                Select status
                    Case AppServiceConnectionStatus.AppNotInstalled
                        rootPage.NotifyUser("The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType.ErrorMessage)
                        Return
                    Case AppServiceConnectionStatus.AppUnavailable
                        rootPage.NotifyUser("The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType.ErrorMessage)
                        Return
                    Case AppServiceConnectionStatus.AppServiceUnavailable
                        rootPage.NotifyUser(String.Format("The app AppServicesProvider is installed but it does not provide the app service {0}.", connection.AppServiceName), NotifyType.ErrorMessage)
                        Return
                    Case AppServiceConnectionStatus.Unknown
                        rootPage.NotifyUser("An unkown error occurred while we were trying to open an AppServiceConnection.", NotifyType.ErrorMessage)
                        Return
                End Select

                'Set up the inputs and send a message to the service
                Dim inputs = New ValueSet()
                inputs.Add("minvalue", minValueInput)
                inputs.Add("maxvalue", maxValueInput)
                Dim response As AppServiceResponse = Await connection.SendMessageAsync(inputs)
                If response.Status = AppServiceResponseStatus.Success AndAlso response.Message.ContainsKey("result") Then
                    Dim resultText = response.Message("result").ToString()
                    If Not String.IsNullOrEmpty(resultText) Then
                        Result.Text = resultText
                        rootPage.NotifyUser("App service responded with a result", NotifyType.StatusMessage)
                    Else
                        rootPage.NotifyUser("App service did not respond with a result", NotifyType.ErrorMessage)
                    End If

                    Return
                End If

                Select response.Status
                    Case AppServiceResponseStatus.Failure
                        rootPage.NotifyUser("The service failed to acknowledge the message we sent it. It may have been terminated or it's RequestReceived handler might not be handling incoming messages correctly.", NotifyType.ErrorMessage)
                        Return
                    Case AppServiceResponseStatus.ResourceLimitsExceeded
                        rootPage.NotifyUser("The service exceeded the resources allocated to it and had to be terminated.", NotifyType.ErrorMessage)
                        Return
                    Case AppServiceResponseStatus.Unknown
                        rootPage.NotifyUser("An unkown error occurred while we were trying to send a message to the service.", NotifyType.ErrorMessage)
                        Return
                End Select
            End Using
        End Sub
    End Class
End Namespace
