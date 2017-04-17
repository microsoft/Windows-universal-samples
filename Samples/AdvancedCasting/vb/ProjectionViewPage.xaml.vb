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
Imports System
Imports System.Threading.Tasks
Imports Windows.UI.Core
Imports Windows.UI.ViewManagement
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    Partial Public NotInheritable Class ProjectionViewPage
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
            Dim pmtcs As ProjectedMediaTransportControls = TryCast(Me.Player.TransportControls, ProjectedMediaTransportControls)
            If pmtcs IsNot Nothing Then
                AddHandler pmtcs.StopProjectingButtonClick, AddressOf ProjectionViewPage_StopProjectingButtonClick
            End If

            AddHandler Me.Player.MediaOpened, AddressOf Player_MediaOpened
        End Sub

        Private Sub Player_MediaOpened(sender As Object, e As RoutedEventArgs)
            Me.Player.IsFullWindow = True
            Me.Player.AreTransportControlsEnabled = True
        End Sub

        Private Sub ProjectionViewPage_StopProjectingButtonClick(sender As Object, e As EventArgs)
            Me.StopProjecting()
        End Sub

        Public Async Function SetMediaSource(source As Uri, position As TimeSpan) As Task(Of Boolean)
            Await Me.Player.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                                   Me.Player.Source = source
                                                                                                   Me.Player.Position = position
                                                                                                   Me.Player.Play()
                                                                                               End Sub)
            Return True
        End Function

        Public ReadOnly Property Player As MediaElement
            Get
                Return _player
            End Get
        End Property

        Dim broker As ProjectionViewBroker = Nothing

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            broker = CType(e.Parameter, ProjectionViewBroker)
            broker.ProjectedPage = Me
            AddHandler broker.ProjectionViewPageControl.Released, AddressOf thisViewControl_Released
        End Sub

        Private Async Sub thisViewControl_Released(sender As Object, e As EventArgs)
            RemoveHandler broker.ProjectionViewPageControl.Released, AddressOf thisViewControl_Released
            Await broker.ProjectionViewPageControl.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                                                                                                          Me.broker.NotifyProjectionStopping()
                                                                                                          MainPage.Current.ProjectionViewPageControl = Nothing
                                                                                                      End Sub)
            Me.Player.Stop()
            Me.Player.Source = Nothing
            Window.Current.Close()
        End Sub

        Public Async Sub SwapViews()
            broker.ProjectionViewPageControl.StartViewInUse()
            Await ProjectionManager.SwapDisplaysForViewsAsync(ApplicationView.GetForCurrentView().Id, broker.MainViewId)
            broker.ProjectionViewPageControl.StopViewInUse()
        End Sub

        Private Sub SwapViews_Click(sender As Object, e As RoutedEventArgs)
            SwapViews()
        End Sub

        Public Async Sub StopProjecting()
            broker.NotifyProjectionStopping()
            Await Me.Player.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                                                                                                   Me.Player.Stop()
                                                                                                   Me.Player.Source = Nothing
                                                                                                   broker.ProjectionViewPageControl.StartViewInUse()
                                                                                                   Try
                                                                                                       Await ProjectionManager.StopProjectingAsync(broker.ProjectionViewPageControl.Id, broker.MainViewId)
                                                                                                   Catch
                                                                                                   End Try

                                                                                                   Window.Current.Close()
                                                                                               End Sub)
            broker.ProjectionViewPageControl.StopViewInUse()
        End Sub

        Private Sub StopProjecting_Click(sender As Object, e As RoutedEventArgs)
            StopProjecting()
        End Sub
    End Class
End Namespace
