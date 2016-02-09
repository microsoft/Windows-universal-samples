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
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Documents
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media

Namespace Global.ScreenCasting.Controls

    Public NotInheritable Class ProjectedMediaTransportControls
        Inherits MediaTransportControls

        Public Sub New()
            Me.DefaultStyleKey = GetType(ProjectedMediaTransportControls)
        End Sub

        Protected Overrides Sub OnApplyTemplate()
            MyBase.OnApplyTemplate()
            'Find the Swap Views button and subscribe to the clicked event.
            Dim swapViewsButton As Button = TryCast(GetTemplateChild("SwapViewsButton"), Button)
            AddHandler swapViewsButton.Click, AddressOf SwapViewsButton_Click
            'Find the Stop Projecting button and subscribe to the clicked event.
            Dim stopProjectingButton As Button = TryCast(GetTemplateChild("StopProjectingButton"), Button)
            AddHandler stopProjectingButton.Click, AddressOf StopProjectingButton_Click
            Me.IsCompact = True
        End Sub

        Public Event StopProjectingButtonClick As EventHandler

        Public Event SwapViewsButtonClick As EventHandler

        Private Sub StopProjectingButton_Click(sender As Object, e As RoutedEventArgs)
            RaiseEvent StopProjectingButtonClick(Me, EventArgs.Empty)
        End Sub

        Private Sub SwapViewsButton_Click(sender As Object, e As RoutedEventArgs)
            RaiseEvent SwapViewsButtonClick(Me, EventArgs.Empty)
        End Sub
    End Class
End Namespace
