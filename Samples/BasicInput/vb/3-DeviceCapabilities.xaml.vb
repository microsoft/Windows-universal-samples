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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.BasicInput

    Public NotInheritable Partial Class Scenario3
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
            ' Retrieve information about whether or not a keyboard is present
            Dim kbdCapabilities As Windows.Devices.Input.KeyboardCapabilities = New Windows.Devices.Input.KeyboardCapabilities()
            keyboardText.Text = "Keyboard present = " & kbdCapabilities.KeyboardPresent.ToString()
            ' Retrieve information about the capabilities of the device's mouse.  This includes:
            ' - Whether or not a mouse is present
            ' - The number of buttons on the mouse
            ' - Whether or not the mouse has a vertical scroll wheel
            ' - Whether or not the mouse has a horizontal scroll wheel
            ' - Whether or not the user has elected to swap the mouse buttons, causing
            '   the right mouse button to be the primary button
            Dim mouseCapabilities As Windows.Devices.Input.MouseCapabilities = New Windows.Devices.Input.MouseCapabilities()
            Dim sb As System.Text.StringBuilder = New System.Text.StringBuilder()
            sb.Append("Mouse present = " & mouseCapabilities.MousePresent.ToString() & vbLf)
            sb.Append("Number of buttons = " & mouseCapabilities.NumberOfButtons.ToString() & vbLf)
            sb.Append("Vertical wheel present present = " & mouseCapabilities.VerticalWheelPresent.ToString() & vbLf)
            sb.Append("Horizontal wheel present = " & mouseCapabilities.HorizontalWheelPresent.ToString() & vbLf)
            sb.Append("Buttons swapped = " & mouseCapabilities.SwapButtons.ToString())
            mouseText.Text = sb.ToString()
            ' Retrieve information about the capabilities of the device's mouse.  This includes:
            ' - Whether or not the device supports touch
            ' - The supported number of simultaneous touch contacts
            Dim touchCapabilities As Windows.Devices.Input.TouchCapabilities = New Windows.Devices.Input.TouchCapabilities()
            sb = New System.Text.StringBuilder()
            sb.Append("Touch present = " & touchCapabilities.TouchPresent.ToString() & vbLf)
            sb.Append("Touch contacts supported = " & touchCapabilities.Contacts.ToString())
            touchText.Text = sb.ToString()
        End Sub
    End Class
End Namespace
