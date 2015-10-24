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
Imports Windows.Foundation
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media

Namespace Global.BasicInput

    Partial Public NotInheritable Class Scenario4
        Inherits Page

        Private transforms As TransformGroup

        Private previousTransform As MatrixTransform

        Private deltaTransform As CompositeTransform

        Private forceManipulationsToEnd As Boolean

        Public Sub New()
            Me.InitializeComponent()
            forceManipulationsToEnd = False
            InitOptions()
            InitManipulationTransforms()
            AddHandler manipulateMe.ManipulationStarted, New ManipulationStartedEventHandler(AddressOf ManipulateMe_ManipulationStarted)
            AddHandler manipulateMe.ManipulationDelta, New ManipulationDeltaEventHandler(AddressOf ManipulateMe_ManipulationDelta)
            AddHandler manipulateMe.ManipulationCompleted, New ManipulationCompletedEventHandler(AddressOf ManipulateMe_ManipulationCompleted)
            AddHandler manipulateMe.ManipulationInertiaStarting, New ManipulationInertiaStartingEventHandler(AddressOf ManipulateMe_ManipulationInertiaStarting)
            manipulateMe.ManipulationMode = ManipulationModes.TranslateX Or ManipulationModes.TranslateY Or ManipulationModes.Rotate Or ManipulationModes.TranslateInertia Or ManipulationModes.RotateInertia
        End Sub

        Private Sub InitManipulationTransforms()
            transforms = New TransformGroup()
            previousTransform = New MatrixTransform() With {.Matrix = Matrix.Identity}
            deltaTransform = New CompositeTransform()
            transforms.Children.Add(previousTransform)
            transforms.Children.Add(deltaTransform)
            manipulateMe.RenderTransform = transforms
        End Sub

        Sub ManipulateMe_ManipulationStarted(sender As Object, e As ManipulationStartedRoutedEventArgs)
            forceManipulationsToEnd = False
            manipulateMe.Background = New SolidColorBrush(Windows.UI.Colors.DeepSkyBlue)
        End Sub

        Sub ManipulateMe_ManipulationDelta(sender As Object, e As ManipulationDeltaRoutedEventArgs)
            If forceManipulationsToEnd Then
                e.Complete()
                Return
            End If

            previousTransform.Matrix = transforms.Value
            ' Get center point for rotation
            Dim center As Point = previousTransform.TransformPoint(New Point(e.Position.X, e.Position.Y))
            deltaTransform.CenterX = center.X
            deltaTransform.CenterY = center.Y
            deltaTransform.Rotation = e.Delta.Rotation
            deltaTransform.TranslateX = e.Delta.Translation.X
            deltaTransform.TranslateY = e.Delta.Translation.Y
        End Sub

        Sub ManipulateMe_ManipulationInertiaStarting(sender As Object, e As ManipulationInertiaStartingRoutedEventArgs)
            manipulateMe.Background = New SolidColorBrush(Windows.UI.Colors.RoyalBlue)
        End Sub

        Sub ManipulateMe_ManipulationCompleted(sender As Object, e As ManipulationCompletedRoutedEventArgs)
            manipulateMe.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End Sub

        Private Sub movementAxis_Changed(sender As Object, e As SelectionChangedEventArgs)
            manipulateMe.ManipulationMode = manipulateMe.ManipulationMode Or ManipulationModes.TranslateX Or ManipulationModes.TranslateY
            Dim selectedItem As ComboBoxItem = CType((CType(sender, ComboBox)).SelectedItem, ComboBoxItem)
            Select Case selectedItem.Content.ToString()
                Case "X only"
                    manipulateMe.ManipulationMode = manipulateMe.ManipulationMode Xor ManipulationModes.TranslateY
                Case "Y only"
                    manipulateMe.ManipulationMode = manipulateMe.ManipulationMode Xor ManipulationModes.TranslateX
            End Select
        End Sub

        Private Sub InertiaSwitch_Toggled(sender As Object, e As RoutedEventArgs)
            If manipulateMe IsNot Nothing Then
                manipulateMe.ManipulationMode = manipulateMe.ManipulationMode Xor ManipulationModes.TranslateInertia Or ManipulationModes.RotateInertia
            End If
        End Sub

        '
        ' More UI code below
        '
        Private Sub InitOptions()
            movementAxis.SelectedIndex = 0
            inertiaSwitch.IsOn = True
        End Sub

        Sub resetButton_Pressed(sender As Object, e As RoutedEventArgs)
            forceManipulationsToEnd = True
            manipulateMe.RenderTransform = Nothing
            movementAxis.SelectedIndex = 0
            InitOptions()
            InitManipulationTransforms()
        End Sub
    End Class
End Namespace
