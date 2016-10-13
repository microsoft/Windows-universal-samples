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
Imports ScreenCasting.Controls
Imports ScreenCasting.Data.Azure
Imports ScreenCasting.Data.Common
Imports ScreenCasting.Util
Imports System
Imports Windows.ApplicationModel.Core
Imports Windows.Devices.Enumeration
Imports Windows.Foundation
Imports Windows.UI.Core
Imports Windows.UI.ViewManagement
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    Partial Public NotInheritable Class Scenario05
        Inherits Page

        Private rootPage As MainPage

        Private picker As DevicePicker

        Private video As VideoMetaData = Nothing

        Dim pvb As ProjectionViewBroker = New ProjectionViewBroker()

        Dim activeDevice As DeviceInformation = Nothing

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
            AddHandler(CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)).CastButtonClicked, AddressOf TransportControls_CastButtonClicked
            picker = New DevicePicker()
            picker.Filter.SupportedDeviceSelectors.Add(ProjectionManager.GetDeviceSelector())
            AddHandler picker.DeviceSelected, AddressOf Picker_DeviceSelected
            AddHandler picker.DisconnectButtonClicked, AddressOf Picker_DisconnectButtonClicked
            AddHandler picker.DevicePickerDismissed, AddressOf Picker_DevicePickerDismissed
            AddHandler pvb.ProjectionStopping, AddressOf Pvb_ProjectionStopping
        End Sub

        Private Sub TransportControls_CastButtonClicked(sender As Object, e As EventArgs)
            rootPage.NotifyUser("Custom Cast Button Clicked", NotifyType.StatusMessage)
            player.Pause()
            'Get the custom transport controls
            Dim mtc As MediaTransportControlsWithCustomCastButton = CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)
            'Retrieve the location of the casting button
            Dim transform As GeneralTransform = mtc.CastButton.TransformToVisual(TryCast(Window.Current.Content, UIElement))
            Dim pt As Point = transform.TransformPoint(New Point(0, 0))
            picker.Show(New Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above)
        End Sub

        Private Async Sub Picker_DeviceSelected(sender As DevicePicker, args As DeviceSelectedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                                                                                         Try
                                                                                             picker.SetDisplayStatus(args.SelectedDevice, "Connecting", DevicePickerDisplayStatusOptions.ShowProgress)
                                                                                             ' Getting the selected device improves debugging
                                                                                             Dim selectedDevice As DeviceInformation = args.SelectedDevice
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
                                                                                                     Await ProjectionManager.StartProjectingAsync(rootPage.ProjectionViewPageControl.Id, thisViewId, selectedDevice)
                                                                                                 Catch ex As Exception
                                                                                                     If Not ProjectionManager.ProjectionDisplayAvailable OrElse pvb.ProjectedPage Is Nothing Then
                                                                                                         Throw ex
                                                                                                     End If

                                                                                                 End Try

                                                                                                 If ProjectionManager.ProjectionDisplayAvailable AndAlso pvb.ProjectedPage IsNot Nothing Then
                                                                                                     Me.player.Pause()
                                                                                                     Await pvb.ProjectedPage.SetMediaSource(Me.player.Source, Me.player.Position)
                                                                                                     activeDevice = selectedDevice
                                                                                                     picker.SetDisplayStatus(args.SelectedDevice, "Connected", DevicePickerDisplayStatusOptions.ShowDisconnectButton)
                                                                                                     picker.Hide()
                                                                                                 Else
                                                                                                     rootPage.NotifyUser(String.Format("Projection has failed to '{0}'", selectedDevice.Name), NotifyType.ErrorMessage)
                                                                                                     picker.SetDisplayStatus(args.SelectedDevice, "Connection Failed", DevicePickerDisplayStatusOptions.ShowRetryButton)
                                                                                                 End If
                                                                                             Catch
                                                                                                 rootPage.NotifyUser(String.Format("Projection has failed to '{0}'", selectedDevice.Name), NotifyType.ErrorMessage)
                                                                                                 Try
                                                                                                     picker.SetDisplayStatus(args.SelectedDevice, "Connection Failed", DevicePickerDisplayStatusOptions.ShowRetryButton)
                                                                                                 Catch
                                                                                                 End Try

                                                                                             End Try
                                                                                         Catch ex As Exception
                                                                                             UnhandledExceptionPage.ShowUnhandledException(ex)
                                                                                         End Try
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
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                         rootPage.NotifyUser("Disconnect Button clicked", NotifyType.StatusMessage)
                                                                                         sender.SetDisplayStatus(args.Device, "Disconnecting", DevicePickerDisplayStatusOptions.ShowProgress)
                                                                                         If Me.pvb.ProjectedPage IsNot Nothing Then
                                                                                             Me.pvb.ProjectedPage.StopProjecting()
                                                                                         End If

                                                                                         sender.SetDisplayStatus(args.Device, "Disconnected", DevicePickerDisplayStatusOptions.None)
                                                                                         rootPage.NotifyUser("Disconnected", NotifyType.StatusMessage)
                                                                                         activeDevice = Nothing
                                                                                     End Sub)
        End Sub

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
