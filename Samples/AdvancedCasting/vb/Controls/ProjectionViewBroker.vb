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
Imports ScreenCasting.Util
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.UI.Core
Imports Windows.UI.Xaml.Controls

Namespace Global.ScreenCasting

    ' This is a simple container for a set of data used to comminicate between the main and the projection view
    Friend Class ProjectionViewBroker

        Public MainPageDispatcher As CoreDispatcher

        Public ProjectionViewPageControl As ViewLifetimeControl

        Public MainViewId As Integer

        Public Event ProjectionStopping As EventHandler

        Public ProjectedPage As ProjectionViewPage

        Public Sub NotifyProjectionStopping()
            Try
                RaiseEvent ProjectionStopping(Me, EventArgs.Empty)
            Catch
            End Try
        End Sub
    End Class
End Namespace
