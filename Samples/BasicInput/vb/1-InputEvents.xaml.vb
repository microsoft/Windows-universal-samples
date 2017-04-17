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
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media

Namespace Global.BasicInput

    Public NotInheritable Partial Class Scenario1
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
            AddHandler pressedTarget.PointerPressed, New PointerEventHandler(AddressOf target_PointerPressed)
            AddHandler pressedTarget.PointerReleased, New PointerEventHandler(AddressOf target_PointerReleased)
            AddHandler enterExitTarget.PointerEntered, New PointerEventHandler(AddressOf target_PointerEntered)
            AddHandler enterExitTarget.PointerExited, New PointerEventHandler(AddressOf target_PointerExited)
            AddHandler tapTarget.Tapped, New TappedEventHandler(AddressOf target_Tapped)
            AddHandler tapTarget.DoubleTapped, New DoubleTappedEventHandler(AddressOf target_DoubleTapped)
            AddHandler holdTarget.Holding, New HoldingEventHandler(AddressOf target_Holding)
            AddHandler holdTarget.RightTapped, New RightTappedEventHandler(AddressOf target_RightTapped)
        End Sub

        Sub target_PointerPressed(sender As Object, e As PointerRoutedEventArgs)
            pressedTarget.Background = New SolidColorBrush(Windows.UI.Colors.RoyalBlue)
            pressedTargetText.Text = "Pointer Pressed"
        End Sub

        Sub target_PointerReleased(sender As Object, e As PointerRoutedEventArgs)
            pressedTarget.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            pressedTargetText.Text = "Pointer Released"
        End Sub

        Sub target_PointerEntered(sender As Object, e As PointerRoutedEventArgs)
            enterExitTarget.Background = New SolidColorBrush(Windows.UI.Colors.RoyalBlue)
            enterExitTargetText.Text = "Pointer Entered"
        End Sub

        Sub target_PointerExited(sender As Object, e As PointerRoutedEventArgs)
            enterExitTarget.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            enterExitTargetText.Text = "Pointer Exited"
        End Sub

        Sub target_Tapped(sender As Object, e As TappedRoutedEventArgs)
            tapTarget.Background = New SolidColorBrush(Windows.UI.Colors.DeepSkyBlue)
            tapTargetText.Text = "Tapped"
        End Sub

        Sub target_DoubleTapped(sender As Object, e As DoubleTappedRoutedEventArgs)
            tapTarget.Background = New SolidColorBrush(Windows.UI.Colors.RoyalBlue)
            tapTargetText.Text = "Double-Tapped"
        End Sub

        Sub target_RightTapped(sender As Object, e As RightTappedRoutedEventArgs)
            holdTarget.Background = New SolidColorBrush(Windows.UI.Colors.RoyalBlue)
            holdTargetText.Text = "Right Tapped"
        End Sub

        Sub target_Holding(sender As Object, e As HoldingRoutedEventArgs)
            If e.HoldingState = Windows.UI.Input.HoldingState.Started Then
                holdTarget.Background = New SolidColorBrush(Windows.UI.Colors.DeepSkyBlue)
                holdTargetText.Text = "Holding"
            ElseIf e.HoldingState = Windows.UI.Input.HoldingState.Completed Then
                holdTarget.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
                holdTargetText.Text = "Held"
            Else
                holdTarget.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
                holdTargetText.Text = "Hold Canceled"
            End If
        End Sub
    End Class
End Namespace
