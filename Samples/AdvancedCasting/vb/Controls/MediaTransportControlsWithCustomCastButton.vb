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

    Public NotInheritable Class MediaTransportControlsWithCustomCastButton
        Inherits MediaTransportControls

        Public Sub New()
            Me.DefaultStyleKey = GetType(MediaTransportControlsWithCustomCastButton)
        End Sub

        Public Event CastButtonClicked As EventHandler

        Private Sub CastButton_Click(sender As Object, e As RoutedEventArgs)
            RaiseEvent CastButtonClicked(Me, EventArgs.Empty)
        End Sub

        Public ReadOnly Property CastButton As Button
            Get
                Return TryCast(Me.GetTemplateChild("CustomCastButton"), Button)
            End Get
        End Property

        Protected Overrides Sub OnApplyTemplate()
            MyBase.OnApplyTemplate()
            Me.IsCompact = True
            AddHandler CastButton.Click, AddressOf CastButton_Click
        End Sub
    End Class
End Namespace
