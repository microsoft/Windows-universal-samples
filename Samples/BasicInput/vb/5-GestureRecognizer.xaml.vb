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
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Input

Namespace Global.BasicInput

    Public NotInheritable Partial Class Scenario5
        Inherits Page

        Dim recognizer As GestureRecognizer

        Dim manipulationProcessor As ManipulationInputProcessor

        Public Sub New()
            Me.InitializeComponent()
            InitOptions()
            recognizer = New GestureRecognizer()
            manipulationProcessor = New ManipulationInputProcessor(recognizer, manipulateMe, mainCanvas)
        End Sub

        Private Sub InitOptions()
            movementAxis.SelectedIndex = 0
            InertiaSwitch.IsOn = True
        End Sub

        Private Sub movementAxis_Changed(sender As Object, e As SelectionChangedEventArgs)
            If manipulationProcessor Is Nothing Then
                Return
            End If

            Dim selectedItem As ComboBoxItem = CType((CType(sender, ComboBox)).SelectedItem, ComboBoxItem)
            Select selectedItem.Content.ToString()
                Case "X only"
                    manipulationProcessor.LockToXAxis()
                     Case "Y only"
                    manipulationProcessor.LockToYAxis()
                     Case Else
                    manipulationProcessor.MoveOnXAndYAxes()
                     End Select
        End Sub

        Private Sub InertiaSwitch_Toggled(sender As Object, e As RoutedEventArgs)
            If manipulationProcessor Is Nothing Then
                Return
            End If

            manipulationProcessor.UseInertia(InertiaSwitch.IsOn)
        End Sub

        Sub resetButton_Pressed(sender As Object, e As RoutedEventArgs)
            InitOptions()
            manipulationProcessor.Reset()
        End Sub
    End Class

    Class ManipulationInputProcessor

        Dim recognizer As GestureRecognizer

        Dim element As UIElement

        Dim reference As UIElement

        Dim cumulativeTransform As TransformGroup

        Dim previousTransform As MatrixTransform

        Dim deltaTransform As CompositeTransform

        Public Sub New(gestureRecognizer As GestureRecognizer, target As UIElement, referenceFrame As UIElement)
            recognizer = gestureRecognizer
            element = target
            reference = referenceFrame
            InitializeTransforms()
            recognizer.GestureSettings = GenerateDefaultSettings()
            AddHandler element.PointerPressed, AddressOf OnPointerPressed
            AddHandler element.PointerMoved, AddressOf OnPointerMoved
            AddHandler element.PointerReleased, AddressOf OnPointerReleased
            AddHandler element.PointerCanceled, AddressOf OnPointerCanceled
            AddHandler recognizer.ManipulationStarted, AddressOf OnManipulationStarted
            AddHandler recognizer.ManipulationUpdated, AddressOf OnManipulationUpdated
            AddHandler recognizer.ManipulationCompleted, AddressOf OnManipulationCompleted
            AddHandler recognizer.ManipulationInertiaStarting, AddressOf OnManipulationInertiaStarting
        End Sub

        Public Sub InitializeTransforms()
            cumulativeTransform = New TransformGroup()
            deltaTransform = New CompositeTransform()
            previousTransform = New MatrixTransform() With {.Matrix = Matrix.Identity}
            cumulativeTransform.Children.Add(previousTransform)
            cumulativeTransform.Children.Add(deltaTransform)
            element.RenderTransform = cumulativeTransform
        End Sub

        Function GenerateDefaultSettings() As GestureSettings
            Return GestureSettings.ManipulationTranslateX Or GestureSettings.ManipulationTranslateY Or GestureSettings.ManipulationRotate Or GestureSettings.ManipulationTranslateInertia Or GestureSettings.ManipulationRotateInertia
        End Function

        Sub OnPointerPressed(sender As Object, args As PointerRoutedEventArgs)
            element.CapturePointer(args.Pointer)
            recognizer.ProcessDownEvent(args.GetCurrentPoint(reference))
        End Sub

        Sub OnPointerMoved(sender As Object, args As PointerRoutedEventArgs)
            recognizer.ProcessMoveEvents(args.GetIntermediatePoints(reference))
        End Sub

        Sub OnPointerReleased(sender As Object, args As PointerRoutedEventArgs)
            recognizer.ProcessUpEvent(args.GetCurrentPoint(reference))
            element.ReleasePointerCapture(args.Pointer)
        End Sub

        Sub OnPointerCanceled(sender As Object, args As PointerRoutedEventArgs)
            recognizer.CompleteGesture()
            element.ReleasePointerCapture(args.Pointer)
        End Sub

        Sub OnManipulationStarted(sender As Object, e As ManipulationStartedEventArgs)
            Dim b As Border = TryCast(element, Border)
            b.Background = New SolidColorBrush(Windows.UI.Colors.DeepSkyBlue)
        End Sub

        Sub OnManipulationUpdated(sender As Object, e As ManipulationUpdatedEventArgs)
            previousTransform.Matrix = cumulativeTransform.Value
            ' Get the center point of the manipulation for rotation
            Dim center As Point = New Point(e.Position.X, e.Position.Y)
            deltaTransform.CenterX = center.X
            deltaTransform.CenterY = center.Y
            deltaTransform.Rotation = e.Delta.Rotation
            deltaTransform.TranslateX = e.Delta.Translation.X
            deltaTransform.TranslateY = e.Delta.Translation.Y
        End Sub

        Sub OnManipulationInertiaStarting(sender As Object, e As ManipulationInertiaStartingEventArgs)
            Dim b As Border = TryCast(element, Border)
            b.Background = New SolidColorBrush(Windows.UI.Colors.RoyalBlue)
        End Sub

        Sub OnManipulationCompleted(sender As Object, e As ManipulationCompletedEventArgs)
            Dim b As Border = TryCast(element, Border)
            b.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End Sub

        ' Modify the GestureSettings property to only allow movement on the X axis
        Public Sub LockToXAxis()
            recognizer.CompleteGesture()
            recognizer.GestureSettings = recognizer.GestureSettings OrElse GestureSettings.ManipulationTranslateY Or GestureSettings.ManipulationTranslateX
            recognizer.GestureSettings = recognizer.GestureSettings Xor GestureSettings.ManipulationTranslateY
        End Sub

        ' Modify the GestureSettings property to only allow movement on the Y axis
        Public Sub LockToYAxis()
            recognizer.CompleteGesture()
            recognizer.GestureSettings = recognizer.GestureSettings OrElse GestureSettings.ManipulationTranslateY Or GestureSettings.ManipulationTranslateX
            recognizer.GestureSettings = recognizer.GestureSettings Xor GestureSettings.ManipulationTranslateX
        End Sub

        ' Modify the GestureSettings property to allow movement on both the the X and Y axes
        Public Sub MoveOnXAndYAxes()
            recognizer.CompleteGesture()
            recognizer.GestureSettings = recognizer.GestureSettings OrElse GestureSettings.ManipulationTranslateX Or GestureSettings.ManipulationTranslateY
        End Sub

        ' Modify the GestureSettings property to enable or disable inertia based on the passed-in value
        Public Sub UseInertia(inertia As Boolean)
            If Not inertia Then
                recognizer.CompleteGesture()
                recognizer.GestureSettings = recognizer.GestureSettings Xor GestureSettings.ManipulationTranslateInertia Or GestureSettings.ManipulationRotateInertia
            Else
                recognizer.GestureSettings = recognizer.GestureSettings OrElse GestureSettings.ManipulationTranslateInertia Or GestureSettings.ManipulationRotateInertia
            End If
        End Sub

        Public Sub Reset()
            element.RenderTransform = Nothing
            recognizer.CompleteGesture()
            InitializeTransforms()
            recognizer.GestureSettings = GenerateDefaultSettings()
        End Sub
    End Class
End Namespace
