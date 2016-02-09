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
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media

Namespace Global.BasicInput

    Public NotInheritable Partial Class Scenario2
        Inherits Page

        Dim popups As Dictionary(Of UInteger, StackPanel)

        Public Sub New()
            Me.InitializeComponent()
            popups = New Dictionary(Of UInteger, StackPanel)()
            AddHandler mainCanvas.PointerPressed, New PointerEventHandler(AddressOf Pointer_Pressed)
            AddHandler mainCanvas.PointerMoved, New PointerEventHandler(AddressOf Pointer_Moved)
            AddHandler mainCanvas.PointerReleased, New PointerEventHandler(AddressOf Pointer_Released)
            AddHandler mainCanvas.PointerExited, New PointerEventHandler(AddressOf Pointer_Released)
            AddHandler mainCanvas.PointerEntered, New PointerEventHandler(AddressOf Pointer_Entered)
            AddHandler mainCanvas.PointerWheelChanged, New PointerEventHandler(AddressOf Pointer_Wheel_Changed)
        End Sub

        Sub Pointer_Pressed(sender As Object, e As PointerRoutedEventArgs)
            Dim currentPoint As Windows.UI.Input.PointerPoint = e.GetCurrentPoint(mainCanvas)
            CreateOrUpdatePropertyPopUp(currentPoint)
        End Sub

        Sub Pointer_Entered(sender As Object, e As PointerRoutedEventArgs)
            Dim currentPoint As Windows.UI.Input.PointerPoint = e.GetCurrentPoint(mainCanvas)
            If currentPoint.IsInContact Then
                CreateOrUpdatePropertyPopUp(currentPoint)
            End If
        End Sub

        Sub Pointer_Moved(sender As Object, e As PointerRoutedEventArgs)
            ' Retrieve the point associated with the current event
            Dim currentPoint As Windows.UI.Input.PointerPoint = e.GetCurrentPoint(mainCanvas)
            If currentPoint.IsInContact Then
                CreateOrUpdatePropertyPopUp(currentPoint)
            End If
        End Sub

        Sub Pointer_Released(sender As Object, e As PointerRoutedEventArgs)
            ' Retrieve the point associated with the current event
            Dim currentPoint As Windows.UI.Input.PointerPoint = e.GetCurrentPoint(mainCanvas)
            HidePropertyPopUp(currentPoint)
        End Sub

        Sub Pointer_Wheel_Changed(sender As Object, e As PointerRoutedEventArgs)
            ' Retrieve the point associated with the current event and record the
            ' mouse wheel delta
            Dim currentPoint As Windows.UI.Input.PointerPoint = e.GetCurrentPoint(mainCanvas)
            If currentPoint.IsInContact Then
                CreateOrUpdatePropertyPopUp(currentPoint)
            End If
        End Sub

        Sub CreateOrUpdatePropertyPopUp(currentPoint As Windows.UI.Input.PointerPoint)
            ' Retrieve the properties that are common to all pointers
            Dim pointerProperties As String = GetPointerProperties(currentPoint)
            ' Retrieve the properties that are specific to the device type associated
            ' with the current PointerPoint
            Dim deviceSpecificProperties As String = GetDeviceSpecificProperties(currentPoint)
            RenderPropertyPopUp(pointerProperties, deviceSpecificProperties, currentPoint)
        End Sub

        Function GetPointerProperties(currentPoint As Windows.UI.Input.PointerPoint) As String
            Dim sb As System.Text.StringBuilder = New System.Text.StringBuilder()
            sb.Append("ID: " & currentPoint.PointerId)
            sb.Append(vbLf & "X: " & currentPoint.Position.X)
            sb.Append(vbLf & "Y: " & currentPoint.Position.Y)
            sb.Append(vbLf & "Contact: " & currentPoint.IsInContact & vbLf)
            Return sb.ToString()
             End Function

        Function GetDeviceSpecificProperties(currentPoint As Windows.UI.Input.PointerPoint) As String
            Dim deviceSpecificProperties As String = ""
            Select currentPoint.PointerDevice.PointerDeviceType
                Case Windows.Devices.Input.PointerDeviceType.Mouse
                    deviceSpecificProperties = GetMouseProperties(currentPoint)
                     Case Windows.Devices.Input.PointerDeviceType.Pen
                    deviceSpecificProperties = GetPenProperties(currentPoint)
                     Case Windows.Devices.Input.PointerDeviceType.Touch
                    deviceSpecificProperties = GetTouchProperties(currentPoint)
                     End Select

            Return deviceSpecificProperties
        End Function

        Function GetMouseProperties(currentPoint As Windows.UI.Input.PointerPoint) As String
            Dim pointerProperties As Windows.UI.Input.PointerPointProperties = currentPoint.Properties
            Dim sb As System.Text.StringBuilder = New System.Text.StringBuilder()
            sb.Append("Left button: " & pointerProperties.IsLeftButtonPressed)
            sb.Append(vbLf & "Right button: " & pointerProperties.IsRightButtonPressed)
            sb.Append(vbLf & "Middle button: " & pointerProperties.IsMiddleButtonPressed)
            sb.Append(vbLf & "X1 button: " & pointerProperties.IsXButton1Pressed)
            sb.Append(vbLf & "X2 button: " & pointerProperties.IsXButton2Pressed)
            sb.Append(vbLf & "Mouse wheel delta: " & pointerProperties.MouseWheelDelta)
            Return sb.ToString()
        End Function

        Function GetTouchProperties(currentPoint As Windows.UI.Input.PointerPoint) As String
            Dim pointerProperties As Windows.UI.Input.PointerPointProperties = currentPoint.Properties
            Dim sb As System.Text.StringBuilder = New System.Text.StringBuilder()
            sb.Append("Contact Rect X: " & pointerProperties.ContactRect.X)
            sb.Append(vbLf & "Contact Rect Y: " & pointerProperties.ContactRect.Y)
            sb.Append(vbLf & "Contact Rect Width: " & pointerProperties.ContactRect.Width)
            sb.Append(vbLf & "Contact Rect Height: " & pointerProperties.ContactRect.Height)
            Return sb.ToString()
             End Function

        Function GetPenProperties(currentPoint As Windows.UI.Input.PointerPoint) As String
            Dim pointerProperties As Windows.UI.Input.PointerPointProperties = currentPoint.Properties
            Dim sb As System.Text.StringBuilder = New System.Text.StringBuilder()
            sb.Append("Barrel button: " & pointerProperties.IsBarrelButtonPressed)
            sb.Append(vbLf & "Eraser: " & pointerProperties.IsEraser)
            sb.Append(vbLf & "Pressure: " & pointerProperties.Pressure)
            Return sb.ToString()
        End Function

        Sub RenderPropertyPopUp(pointerProperties As String, deviceSpecificProperties As String, currentPoint As Windows.UI.Input.PointerPoint)
            If popups.ContainsKey(currentPoint.PointerId) Then
                Dim pointerText As TextBlock = CType(popups(currentPoint.PointerId).Children(0), TextBlock)
                pointerText.Text = pointerProperties
                Dim deviceSpecificText As TextBlock = CType(popups(currentPoint.PointerId).Children(1), TextBlock)
                deviceSpecificText.Text = deviceSpecificProperties
            Else
                Dim pointerPanel As StackPanel = New StackPanel()
                Dim pointerText As TextBlock = New TextBlock()
                pointerText.Text = pointerProperties
                pointerPanel.Children.Add(pointerText)
                Dim deviceSpecificText As TextBlock = New TextBlock()
                deviceSpecificText.Text = deviceSpecificProperties
                Select currentPoint.PointerDevice.PointerDeviceType
                    Case Windows.Devices.Input.PointerDeviceType.Mouse
                        deviceSpecificText.Foreground = New SolidColorBrush(Windows.UI.Colors.Red)
                         Case Windows.Devices.Input.PointerDeviceType.Touch
                        deviceSpecificText.Foreground = New SolidColorBrush(Windows.UI.Colors.Green)
                         Case Windows.Devices.Input.PointerDeviceType.Pen
                        deviceSpecificText.Foreground = New SolidColorBrush(Windows.UI.Colors.Yellow)
                         End Select

                pointerPanel.Children.Add(deviceSpecificText)
                popups(currentPoint.PointerId) = pointerPanel
                mainCanvas.Children.Add(popups(currentPoint.PointerId))
            End If

            Dim transform As TranslateTransform = New TranslateTransform()
            transform.X = currentPoint.Position.X + 24
            transform.Y = currentPoint.Position.Y + 24
            popups(currentPoint.PointerId).RenderTransform = transform
        End Sub

        Sub HidePropertyPopUp(currentPoint As Windows.UI.Input.PointerPoint)
            If popups.ContainsKey(currentPoint.PointerId) Then
                mainCanvas.Children.Remove(popups(currentPoint.PointerId))
                popups(currentPoint.PointerId) = Nothing
                popups.Remove(currentPoint.PointerId)
            End If
        End Sub
    End Class
End Namespace
