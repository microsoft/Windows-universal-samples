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
Imports System
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate
Imports Windows.ApplicationModel.AppService
Imports Windows.Foundation.Collections

Namespace Global.AppServicesClient

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class KeepConnectionOpenScenario
        Inherits Page

        Private rootPage As MainPage

        Private connection As AppServiceConnection

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Private Async Sub OpenConnection_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            If connection IsNot Nothing Then
                rootPage.NotifyUser("A connection already exists", NotifyType.ErrorMessage)
                Return
            End If

            connection = New AppServiceConnection()
            connection.AppServiceName = "com.microsoft.randomnumbergenerator"
            connection.PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CS_8wekyb3d8bbwe"
            AddHandler connection.ServiceClosed, AddressOf Connection_ServiceClosed
            Dim status As AppServiceConnectionStatus = Await connection.OpenAsync()
            If status = AppServiceConnectionStatus.Success Then
                rootPage.NotifyUser("Connection is open", NotifyType.StatusMessage)
            Else
                Select Case status
                    Case AppServiceConnectionStatus.AppNotInstalled
                        rootPage.NotifyUser("The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType.ErrorMessage)
                    Case AppServiceConnectionStatus.AppUnavailable
                        rootPage.NotifyUser("The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType.ErrorMessage)
                    Case AppServiceConnectionStatus.AppServiceUnavailable
                        rootPage.NotifyUser(String.Format("The app AppServicesProvider is installed but it does not provide the app service {0}.", connection.AppServiceName), NotifyType.ErrorMessage)
                    Case AppServiceConnectionStatus.Unknown
                        rootPage.NotifyUser("An unkown error occurred while we were trying to open an AppServiceConnection.", NotifyType.ErrorMessage)
                End Select

                connection.Dispose()
                connection = Nothing
            End If
        End Sub

        Private Async Sub Connection_ServiceClosed(sender As AppServiceConnection, args As AppServiceClosedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                         If connection IsNot Nothing Then
                                                                                             connection.Dispose()
                                                                                             connection = Nothing
                                                                                         End If
                                                                                     End Sub)
        End Sub

        Private Sub CloseConnection_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            If connection Is Nothing Then
                rootPage.NotifyUser("There's no open connection to close", NotifyType.ErrorMessage)
                Return
            End If

            connection.Dispose()
            connection = Nothing
            rootPage.NotifyUser("Connection is closed", NotifyType.StatusMessage)
        End Sub

        Private Async Sub GenerateRandomNumber_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            If connection Is Nothing Then
                rootPage.NotifyUser("You need to open a connection before trying to generate a random number.", NotifyType.ErrorMessage)
                Return
            End If

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

            'Send a message to the app service
            Dim inputs = New ValueSet()
            inputs.Add("minvalue", minValueInput)
            inputs.Add("maxvalue", maxValueInput)
            Dim response As AppServiceResponse = Await connection.SendMessageAsync(inputs)
            If response.Status = AppServiceResponseStatus.Success Then
                If Not response.Message.ContainsKey("result") Then
                    rootPage.NotifyUser("The app service response message does not contain a key called ""result""", NotifyType.StatusMessage)
                    Return
                End If

                Dim resultText = response.Message("result").ToString()
                If Not String.IsNullOrEmpty(resultText) Then
                    Result.Text = resultText
                    rootPage.NotifyUser("App service responded with a result", NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("App service did not respond with a result", NotifyType.ErrorMessage)
                End If
            Else
                Select Case response.Status
                    Case AppServiceResponseStatus.Failure
                        rootPage.NotifyUser("The service failed to acknowledge the message we sent it. It may have been terminated because the client was suspended.", NotifyType.ErrorMessage)
                    Case AppServiceResponseStatus.ResourceLimitsExceeded
                        rootPage.NotifyUser("The service exceeded the resources allocated to it and had to be terminated.", NotifyType.ErrorMessage)
                    Case Else
                        rootPage.NotifyUser("An unkown error occurred while we were trying to send a message to the service.", NotifyType.ErrorMessage)
                End Select
            End If
        End Sub
    End Class
End Namespace
