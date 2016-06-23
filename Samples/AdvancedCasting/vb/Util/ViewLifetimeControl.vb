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
' The objects defined here demonstrate how to make sure each of the views created remains alive as long as 
' the app needs them, but only when they're being used by the app or the user. Many of the scenarios contained in this
' sample use these functions to keep track of the views available and ensure that the view is not closed while
' the scenario is attempting to show it.
'
' As you can see in scenario 1, the ApplicationViewSwitcher.TryShowAsStandaloneAsync and 
' ProjectionManager.StartProjectingAsync methods let you show one view next to another. The Consolidated event
' is fired when a view stops being visible separately from other views. Common cases where this will occur
' is when the view falls out of the list of recently used apps, or when the user performs the close gesture on the view.
' This is a good time to close the view, provided the app isn't trying to show the view at the same time. This event
' is fired on the thread of the view that becomes consolidated.
'
' Each view lives on its own thread, so concurrency control is necessary. Also, as you'll see in the sample,
' certain objects may be bound to UI on given threads. Properties of those objects should only be updated
' on that UI thread.
Imports System
Imports System.ComponentModel
Imports Windows.UI.Core
Imports Windows.UI.ViewManagement

Namespace Global.ScreenCasting.Util

    ' A custom event that fires whenever the secondary view is ready to be closed. You should
    ' clean up any state (including deregistering for events) then close the window in this handler
    Public Delegate Sub ViewReleasedHandler(sender As [Object], e As EventArgs)

    ' A ViewLifetimeControl is instantiated for every secondary view. ViewLifetimeControl's reference count
    ' keeps track of when the secondary view thinks it's in usem and when the main view is interacting with the secondary view (about to show
    ' it to the user, etc.) When the reference count drops to zero, the secondary view is closed.
    Public NotInheritable Class ViewLifetimeControl
        Implements INotifyPropertyChanged

        Dim window As CoreWindow

        Dim _title As String

        Dim refCount As Integer = 0

        Dim viewId As Integer

        Dim _released As Boolean = False

        Dim _consolidated As Boolean = True

        Event InternalReleased As ViewReleasedHandler

        ' Instantiate views using "CreateForCurrentView"
        Private Sub New(newWindow As CoreWindow)
            _Dispatcher = newWindow.Dispatcher
            window = newWindow
            viewId = ApplicationView.GetApplicationViewIdForWindow(window)
            RegisterForEvents()
        End Sub

        ' Register for events on the current view
        Private Sub RegisterForEvents()
            AddHandler ApplicationView.GetForCurrentView().Consolidated, AddressOf ViewConsolidated
            AddHandler window.VisibilityChanged, AddressOf VisibilityChanged
        End Sub

        ' Unregister for events. Call this method before closing the view to prevent leaks.
        Private Sub UnregisterForEvents()
            RemoveHandler ApplicationView.GetForCurrentView().Consolidated, AddressOf ViewConsolidated
            RemoveHandler window.VisibilityChanged, AddressOf VisibilityChanged
        End Sub

        Private Sub VisibilityChanged(sender As Object, e As VisibilityChangedEventArgs)
            If e.Visible Then
                Consolidated = False
            End If
        End Sub

        ' A view is consolidated with other views hen there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
        ' launched from Start, etc.) A view stops being consolidated when it's visible--at that point the user can interact with it, move it on or off screen, etc. 
        ' It's generally a good idea to close a view after it has been consolidated, but keep it open while it's visible.
        Private Sub ViewConsolidated(sender As ApplicationView, e As ApplicationViewConsolidatedEventArgs)
            Consolidated = True
        End Sub

        ' Called when a view has been "consolidated" (no longer accessible to the user) 
        ' and no other view is trying to interact with it. This should only be closed after the reference
        ' count goes to 0 (including being consolidated). At the end of this, the view is closed. 
        Private Sub FinalizeRelease()
            Dim justReleased As Boolean = False
            SyncLock Me
                If refCount = 0 Then
                    justReleased = True
                    _released = True
                End If

            End SyncLock

            If justReleased Then
                UnregisterForEvents()
                RaiseEvent InternalReleased(Me, Nothing)
            End If
        End Sub

        ' Creates ViewLifetimeControl for the particular view.
        ' Only do this once per view.
        Public Shared Function CreateForCurrentView() As ViewLifetimeControl
            Return New ViewLifetimeControl(CoreWindow.GetForCurrentThread())
        End Function

        ' For purposes of this sample, the collection of views
        ' is bound to a UI collection. This property is available for binding
        Public Property Title As String
            Get
                Return _title
            End Get

            Set
                If _title <> Value Then
                    _title = Value
                    RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs("Title"))
                End If
            End Set
        End Property

        ' Necessary to communicate with the window
        Public ReadOnly Property Dispatcher As CoreDispatcher

        ' Each view has a unique Id, found using the ApplicationView.Id property or
        ' ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in all of the ApplicationViewSwitcher
        ' and ProjectionManager APIs. 
        Public ReadOnly Property Id As Integer
            Get
                Return viewId
            End Get
        End Property

        ' Keeps track of if the consolidated event has fired yet. A view is consolidated with other views
        ' when there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
        ' launched from Start, etc.) A view stops being consolidated when it's visible--at that point
        ' the user can interact with it, move it on or off screen, etc. 
        Public Property Consolidated As Boolean
            Get
                Return _consolidated
            End Get
            Set(value As Boolean)
                If _consolidated <> value Then
                    _consolidated = value
                    If _consolidated Then
                        StopViewInUse()
                    Else
                        StartViewInUse()
                    End If
                End If
            End Set
        End Property

        ' Signals that the view is being interacted with by another view,
        ' so it shouldn't be closed even if it becomes "consolidated"
        Public Function StartViewInUse() As Integer
            Dim releasedCopy As Boolean = False
            Dim refCountCopy As Integer = 0
            SyncLock Me
                releasedCopy = Me._released
                If Not _released Then
                    refCountCopy = "'refCount=refCount+1' could not be converted to a ExpressionSyntax"
                End If

            End SyncLock

            If releasedCopy Then
                Throw New InvalidOperationException("This view is being disposed")
            End If

            Return refCountCopy
        End Function

        ' Should come after any call to StartViewInUse
        ' Signals that the another view has finished interacting with the view tracked
        ' by this object
        Public Function StopViewInUse() As Integer
            Dim refCountCopy As Integer = 0
            Dim releasedCopy As Boolean = False
            SyncLock Me
                releasedCopy = Me._released
                If Not _released Then
                    refCountCopy = "'refCount=refCount-1' could not be converted to a ExpressionSyntax"
                    If refCountCopy = 0 Then
                        Dispatcher.RunAsync(CoreDispatcherPriority.Low, AddressOf FinalizeRelease)
                    End If
                End If

            End SyncLock

            If releasedCopy Then
                Throw New InvalidOperationException("This view is being disposed")
            End If

            Return refCountCopy
        End Function

        ' Signals to consumers that its time to close the view so that
        ' they can clean up (including calling Window.Close() when finished)
        Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

        Public Custom Event Released As ViewReleasedHandler
            AddHandler(value As ViewReleasedHandler)
                Dim releasedCopy As Boolean = False
                SyncLock Me
                    releasedCopy = _released
                    If Not _released Then
                        AddHandler InternalReleased, value
                    End If

                End SyncLock

                If releasedCopy Then
                    Throw New InvalidOperationException("This view is being disposed")
                End If
            End AddHandler

            RemoveHandler(value As ViewReleasedHandler)
                SyncLock Me
                    RemoveHandler InternalReleased, value
                End SyncLock
            End RemoveHandler
            RaiseEvent(sender As Object, e As EventArgs)
                RaiseEvent InternalReleased(sender, e)
            End RaiseEvent
        End Event
    End Class
End Namespace
