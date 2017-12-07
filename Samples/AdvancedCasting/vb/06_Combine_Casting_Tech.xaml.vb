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
Imports Windows.Foundation
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.UI.Xaml.Media
Imports Windows.Media.DialProtocol
Imports ScreenCasting.Data.Azure
Imports ScreenCasting.Data.Common
Imports Windows.Devices.Enumeration
Imports System.Threading.Tasks
Imports Windows.Media.Casting
Imports ScreenCasting.Controls
Imports ScreenCasting.Util
Imports Windows.UI.ViewManagement
Imports Windows.ApplicationModel.Core
Imports Windows.UI.Core
Imports Windows.Storage

Namespace Global.ScreenCasting

    Partial Public NotInheritable Class Scenario06
        Inherits Page

        Private Const MAX_RESULTS As Integer = 10

        Private rootPage As MainPage

        Private picker As DevicePicker = Nothing

        Private activeDevice As DeviceInformation = Nothing

        Private activeCastConnectionHandler As Object = Nothing

        Private video As VideoMetaData = Nothing

        Dim thisViewId As Integer

        Public Sub New()
            Me.InitializeComponent()
            rootPage = MainPage.Current
            AddHandler player.MediaOpened, AddressOf Player_MediaOpened
            AddHandler player.MediaFailed, AddressOf Player_MediaFailed
            AddHandler player.CurrentStateChanged, AddressOf Player_CurrentStateChanged
            ' Get an Azure hosted video
            Dim dataProvider As AzureDataProvider = New AzureDataProvider()
            video = dataProvider.GetRandomVideo()
            rootPage.NotifyUser(String.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage)
            Me.player.Source = video.VideoLink
            Me.LicenseText.Text = "License: " & video.License
            Me.dial_launch_args_textbox.Text = String.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id)
            AddHandler(CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)).CastButtonClicked, AddressOf TransportControls_CastButtonClicked
            picker = New DevicePicker()
            AddHandler picker.DeviceSelected, AddressOf Picker_DeviceSelected
            AddHandler picker.DisconnectButtonClicked, AddressOf Picker_DisconnectButtonClicked
            AddHandler picker.DevicePickerDismissed, AddressOf Picker_DevicePickerDismissed
            picker.Filter.SupportedDeviceSelectors.Add("System.Devices.DevObjectType:=6 AND System.Devices.AepContainer.ProtocolIds:~~{0E261DE4-12F0-46E6-91BA-428607CCEF64} AND System.Devices.AepContainer.Categories:~~Multimedia.ApplicationLauncher.DIAL")
            picker.Filter.SupportedDeviceSelectors.Add("System.Devices.InterfaceClassGuid:=""{D0875FB4-2196-4c7a-A63D-E416ADDD60A1}""" & " AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True")
            picker.Filter.SupportedDeviceSelectors.Add(ProjectionManager.GetDeviceSelector())
            AddHandler pvb.ProjectionStopping, AddressOf Pvb_ProjectionStopping
        End Sub

        Dim pvb As ProjectionViewBroker = New ProjectionViewBroker()

        Private Sub TransportControls_CastButtonClicked(sender As Object, e As EventArgs)
            player.Pause()
            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage)
            'Get the custom transport controls
            Dim mtc As MediaTransportControlsWithCustomCastButton = CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)
            'Retrieve the location of the casting button
            Dim transform As GeneralTransform = mtc.CastButton.TransformToVisual(TryCast(Window.Current.Content, UIElement))
            Dim pt As Point = transform.TransformPoint(New Point(0, 0))
            picker.Show(New Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above)
        End Sub

#Region " Device Picker Methods "
        Private Async Sub Picker_DeviceSelected(sender As DevicePicker, args As DeviceSelectedEventArgs)
            Dim deviceId As String = args.SelectedDevice.Id
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                                                                                         Try
                                                                                             picker.SetDisplayStatus(args.SelectedDevice, "Connecting", DevicePickerDisplayStatusOptions.ShowProgress)
                                                                                         Catch
                                                                                         End Try

                                                                                         'The selectedDeviceInfo instance is needed to be able to update the picker.
                                                                                         Dim selectedDeviceInfo As DeviceInformation = args.SelectedDevice
#If DEBUG Then
                                                                                         selectedDeviceInfo = Await DeviceInformation.CreateFromIdAsync(args.SelectedDevice.Id)
#End If
                                                                                         Dim castSucceeded As Boolean = False
                                                                                         castSucceeded = Await TryLaunchDialAppAsync(selectedDeviceInfo)
                                                                                         If Not castSucceeded Then
                                                                                             castSucceeded = Await TryProjectionManagerCastAsync(selectedDeviceInfo)
                                                                                         End If

                                                                                         If Not castSucceeded Then
                                                                                             castSucceeded = Await TryCastMediaElementAsync(selectedDeviceInfo)
                                                                                         End If

                                                                                         If castSucceeded Then
                                                                                             Try
                                                                                                 picker.SetDisplayStatus(selectedDeviceInfo, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton)
                                                                                             Catch
                                                                                             End Try

                                                                                             Try
                                                                                                 picker.Hide()
                                                                                             Catch
                                                                                             End Try
                                                                                         Else
                                                                                             Try
                                                                                                 picker.SetDisplayStatus(selectedDeviceInfo, "Connecting failed", DevicePickerDisplayStatusOptions.ShowRetryButton)
                                                                                             Catch
                                                                                             End Try
                                                                                         End If
                                                                                     End Sub)
        End Sub

        Private Async Sub Picker_DevicePickerDismissed(sender As DevicePicker, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                         If activeDevice Is Nothing Then
                                                                                             player.Play()
                                                                                         End If
                                                                                     End Sub)
        End Sub

        Private Async Sub Picker_DisconnectButtonClicked(sender As DevicePicker, args As DeviceDisconnectButtonClickedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                                                                                         rootPage.NotifyUser("Disconnect Button clicked", NotifyType.StatusMessage)
                                                                                         sender.SetDisplayStatus(args.Device, "Disconnecting", DevicePickerDisplayStatusOptions.ShowProgress)
                                                                                         Dim disconnected As Boolean = False
                                                                                         If TypeOf Me.activeCastConnectionHandler Is ProjectionViewBroker Then
                                                                                             disconnected = TryStopProjectionManagerAsync(CType(activeCastConnectionHandler, ProjectionViewBroker))
                                                                                         End If

                                                                                         If TypeOf Me.activeCastConnectionHandler Is DialApp Then
                                                                                             disconnected = Await TryStopDialAppAsync(CType(activeCastConnectionHandler, DialApp))
                                                                                         End If

                                                                                         If TypeOf Me.activeCastConnectionHandler Is CastingConnection Then
                                                                                             disconnected = Await TryDisconnectCastingSessionAsync(CType(activeCastConnectionHandler, CastingConnection))
                                                                                         End If

                                                                                         If disconnected Then
                                                                                             Try
                                                                                                 sender.SetDisplayStatus(args.Device, "Disconnected", DevicePickerDisplayStatusOptions.None)
                                                                                             Catch
                                                                                             End Try

                                                                                             activeDevice = Nothing
                                                                                             activeCastConnectionHandler = Nothing
                                                                                             sender.Hide()
                                                                                         Else
                                                                                             sender.SetDisplayStatus(args.Device, "Disconnect failed", DevicePickerDisplayStatusOptions.ShowDisconnectButton)
                                                                                         End If
                                                                                     End Sub)
        End Sub

#End Region
#Region " ProjectionManager APIs"
        Private Async Function TryProjectionManagerCastAsync(device As DeviceInformation) As Task(Of Boolean)
            Dim projectionManagerCastAsyncSucceeded As Boolean = False
            If (activeDevice Is Nothing AndAlso ProjectionManager.ProjectionDisplayAvailable AndAlso device Is Nothing) OrElse device IsNot Nothing Then
                thisViewId = Windows.UI.ViewManagement.ApplicationView.GetForCurrentView().Id
                If rootPage.ProjectionViewPageControl Is Nothing Then
                    ' First, create a new, blank view
                    Dim thisDispatcher = Window.Current.Dispatcher
                    Await CoreApplication.CreateNewView().Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                                                                                                                 rootPage.ProjectionViewPageControl = ViewLifetimeControl.CreateForCurrentView()
                                                                                                                 pvb.MainPageDispatcher = thisDispatcher
                                                                                                                 pvb.ProjectionViewPageControl = rootPage.ProjectionViewPageControl
                                                                                                                 pvb.MainViewId = thisViewId
                                                                                                                 ' Display the page in the view. Note that the view will not become visible
                                                                                                                 ' until "StartProjectingAsync" is called
                                                                                                                 Dim rootFrame = New Frame()
                                                                                                                 rootFrame.Navigate(GetType(ProjectionViewPage), pvb)
                                                                                                                 Window.Current.Content = rootFrame
                                                                                                                 Window.Current.Activate()
                                                                                                             End Sub)
                End If

                Try
                    rootPage.ProjectionViewPageControl.StartViewInUse()
                    Try
                        rootPage.NotifyUser(String.Format("Starting projection of '{0}' on a second view '{1}' using ProjectionManager", video.Title, device.Name), NotifyType.StatusMessage)
                        Await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, device)
                    Catch ex As Exception
                        If Not ProjectionManager.ProjectionDisplayAvailable Then
                            Throw ex
                        End If

                    End Try

                    If pvb.ProjectedPage IsNot Nothing Then
                        Me.player.Pause()
                        Await pvb.ProjectedPage.SetMediaSource(Me.player.Source, Me.player.Position)
                    End If

                    If device IsNot Nothing Then
                        activeDevice = device
                        activeCastConnectionHandler = pvb
                    End If

                    projectionManagerCastAsyncSucceeded = True
                    rootPage.NotifyUser(String.Format("Displaying '{0}' on a second view '{1}' using ProjectionManager", video.Title, device.Name), NotifyType.StatusMessage)
                Catch
                    rootPage.NotifyUser("The projection view is being disposed", NotifyType.ErrorMessage)
                End Try

                ApplicationView.GetForCurrentView().ExitFullScreenMode()
            End If

            Return projectionManagerCastAsyncSucceeded
        End Function

        Private Function TryStopProjectionManagerAsync(broker As ProjectionViewBroker) As Boolean
            broker.ProjectedPage.StopProjecting()
            Return True
        End Function

        Private Async Sub Pvb_ProjectionStopping(sender As Object, e As EventArgs)
            Dim broker As ProjectionViewBroker = TryCast(sender, ProjectionViewBroker)
            Dim position As TimeSpan
            Dim source As Uri = Nothing
            Await broker.ProjectedPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                                              position = broker.ProjectedPage.Player.Position
                                                                                                              source = broker.ProjectedPage.Player.Source
                                                                                                          End Sub)
            Await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                                  rootPage.NotifyUser("Resuming playback on the first screen", NotifyType.StatusMessage)
                                                                                                  Me.player.Source = source
                                                                                                  Me.player.Position = position
                                                                                                  Me.player.Play()
                                                                                                  rootPage.ProjectionViewPageControl = Nothing
                                                                                              End Sub)
        End Sub

#End Region
#Region " Windows.Media.Casting APIs "
        Private Async Function TryCastMediaElementAsync(device As DeviceInformation) As Task(Of Boolean)
            Dim castMediaElementSucceeded As Boolean = False
            rootPage.NotifyUser(String.Format("Checking to see if device {0} supports Miracast, Bluetooth, or DLNA", device.Name), NotifyType.StatusMessage)
            Dim connection As CastingConnection = Nothing
            If activeDevice IsNot Nothing AndAlso device.Id = activeDevice.Id Then
                connection = TryCast(activeCastConnectionHandler, CastingConnection)
            Else
                activeDevice = Nothing
                activeCastConnectionHandler = Nothing
            End If

            If connection Is Nothing OrElse connection.State = CastingConnectionState.Disconnected OrElse connection.State = CastingConnectionState.Disconnecting Then
                Dim castDevice As CastingDevice = Nothing
                activeDevice = Nothing
                rootPage.NotifyUser(String.Format("Attempting to resolve casting device for '{0}'", device.Name), NotifyType.StatusMessage)
                Try
                    castDevice = Await CastingDevice.FromIdAsync(device.Id)
                Catch
                End Try

                If castDevice Is Nothing Then
                    rootPage.NotifyUser(String.Format("'{0}' does not support playback of this media", device.Name), NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser(String.Format("Creating connection for '{0}'", device.Name), NotifyType.StatusMessage)
                    connection = castDevice.CreateCastingConnection()
                    AddHandler connection.ErrorOccurred, AddressOf Connection_ErrorOccurred
                    AddHandler connection.StateChanged, AddressOf Connection_StateChanged
                End If

                rootPage.NotifyUser(String.Format("Casting to '{0}'", device.Name), NotifyType.StatusMessage)
                Dim source As CastingSource = Nothing
                Try
                    source = player.GetAsCastingSource()
                Catch
                End Try

                If source Is Nothing Then
                    rootPage.NotifyUser(String.Format("Failed to get casting source for video '{0}'", video.Title), NotifyType.ErrorMessage)
                Else
                    Dim status As CastingConnectionErrorStatus = Await connection.RequestStartCastingAsync(source)
                    If status = CastingConnectionErrorStatus.Succeeded Then
                        activeDevice = device
                        activeCastConnectionHandler = connection
                        castMediaElementSucceeded = True
                        player.Play()
                    Else
                        rootPage.NotifyUser(String.Format("Failed to cast to '{0}'", device.Name), NotifyType.ErrorMessage)
                    End If
                End If
            End If

            Return castMediaElementSucceeded
        End Function

        Private Async Sub Connection_StateChanged(sender As CastingConnection, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                         rootPage.NotifyUser("Casting Connection State Changed: " & sender.State.ToString(), NotifyType.StatusMessage)
                                                                                     End Sub)
        End Sub

        Private Async Sub Connection_ErrorOccurred(sender As CastingConnection, args As CastingConnectionErrorOccurredEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                         rootPage.NotifyUser("Casting Error Occured: " & args.Message, NotifyType.ErrorMessage)
                                                                                         activeDevice = Nothing
                                                                                         activeCastConnectionHandler = Nothing
                                                                                     End Sub)
        End Sub

        Private Async Function TryDisconnectCastingSessionAsync(connection As CastingConnection) As Task(Of Boolean)
            Dim disconnected As Boolean = False
            'Disconnect the casting session
            Dim status As CastingConnectionErrorStatus = Await connection.DisconnectAsync()
            If status = CastingConnectionErrorStatus.Succeeded Then
                rootPage.NotifyUser("Connection disconnected successfully.", NotifyType.StatusMessage)
                disconnected = True
            Else
                rootPage.NotifyUser(String.Format("Failed to disconnect connection with reason {0}.", status.ToString()), NotifyType.ErrorMessage)
            End If

            Return disconnected
        End Function

#End Region
#Region " Windows.Media.DialProtocal APIs "
        Private Async Function TryLaunchDialAppAsync(device As DeviceInformation) As Task(Of Boolean)
            Dim dialAppLaunchSucceeded As Boolean = False
            Me.dial_launch_args_textbox.Text = String.Format("v={0}&t={1}&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id, player.Position.Ticks)
            rootPage.NotifyUser(String.Format("Checking to see if device {0} supports DIAL", device.Name), NotifyType.StatusMessage)
            Dim dialDevice As DialDevice = Nothing
            rootPage.NotifyUser(String.Format("Attempting to resolve DIAL device for '{0}'", device.Name), NotifyType.StatusMessage)
            Dim newIds As String() = CType(device.Properties("{0BBA1EDE-7566-4F47-90EC-25FC567CED2A} 2"), String())
            If newIds.Length > 0 Then
                Dim deviceId As String = newIds(0)
                Try
                    dialDevice = Await DialDevice.FromIdAsync(deviceId)
                Catch
                End Try
            End If

            If dialDevice Is Nothing Then
                rootPage.NotifyUser(String.Format("'{0}' does not support DIAL", device.Name), NotifyType.StatusMessage)
            Else
                'Get the DialApp object for the specific application on the selected device
                Dim app As DialApp = dialDevice.GetDialApp(Me.dial_appname_textbox.Text)
                If app Is Nothing Then
                    rootPage.NotifyUser(String.Format("'{0}' cannot find app with ID '{1}'", device.Name, Me.dial_appname_textbox.Text), NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser(String.Format("Attempting to launch '{0}'", app.AppName), NotifyType.StatusMessage)
                    'Launch the application on the 1st screen device
                    Dim result As DialAppLaunchResult = Await app.RequestLaunchAsync(Me.dial_launch_args_textbox.Text)
                    If result = DialAppLaunchResult.Launched Then
                        activeDevice = device
                        activeCastConnectionHandler = app
                        rootPage.NotifyUser(String.Format("Launched '{0}'", app.AppName), NotifyType.StatusMessage)
                        dialAppLaunchSucceeded = True
                    End If
                End If
            End If

            Return dialAppLaunchSucceeded
        End Function

        Private Async Function TryStopDialAppAsync(app As DialApp) As Task(Of Boolean)
            Dim stopped As Boolean = False
            'Get the current application state
            Dim stateDetails As DialAppStateDetails = Await app.GetAppStateAsync()
            Select Case stateDetails.State
                Case DialAppState.NetworkFailure
                    rootPage.NotifyUser("Network Failure while getting application state", NotifyType.ErrorMessage)
                    Exit Select
                Case DialAppState.Stopped
                    stopped = True
                    rootPage.NotifyUser("Application was already stopped.", NotifyType.StatusMessage)
                    Exit Select
                Case Else
                    Dim result As DialAppStopResult = Await app.StopAsync()
                    If result = DialAppStopResult.Stopped Then
                        stopped = True
                        rootPage.NotifyUser("Application stopped successfully.", NotifyType.StatusMessage)
                    Else
                        If result = DialAppStopResult.StopFailed OrElse result = DialAppStopResult.NetworkFailure Then
                            rootPage.NotifyUser(String.Format("Error occured trying to stop application. Status: '{0}'", result.ToString()), NotifyType.StatusMessage)
                        Else
                            stopped = True
                            rootPage.NotifyUser(String.Format("Stop is not supported by device: '{0}'", activeDevice.Name), NotifyType.ErrorMessage)
                        End If
                    End If

                    Exit Select
            End Select

            Return stopped
        End Function

#End Region
#Region " Media Element Status Methods "
        Private Sub Player_CurrentStateChanged(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser(String.Format("{0} '{1}'", Me.player.CurrentState, video.Title), NotifyType.StatusMessage)
        End Sub

        Private Sub Player_MediaFailed(sender As Object, e As ExceptionRoutedEventArgs)
            rootPage.NotifyUser(String.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage)
        End Sub

        Private Sub Player_MediaOpened(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser(String.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage)
            player.Play()
        End Sub

#End Region
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub
    End Class
End Namespace
