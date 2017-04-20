
Imports Windows.Devices.Enumeration
Imports Windows.Devices.Sensors
Imports Windows.Graphics.Display
Imports Windows.Media.Capture
Imports Windows.Storage.FileProperties
Namespace CameraStarterKit
    Class CameraRotationHelper
        Private _cameraEnclosureLocation As EnclosureLocation
        Private _displayInformation As DisplayInformation = DisplayInformation.GetForCurrentView()
        Private _orientationSensor As SimpleOrientationSensor = SimpleOrientationSensor.GetDefault()
        ''' <summary>
        ''' Occurs each time the simple orientation sensor reports a new sensor reading or when the display's current or native orientation changes
        ''' </summary>
        Public Event OrientationChanged(ByVal UpdateUI As Boolean)
        Public Sub New(cameraEnclosureLocation As EnclosureLocation)
            _cameraEnclosureLocation = cameraEnclosureLocation
            If Not IsEnclosureLocationExternal(_cameraEnclosureLocation) AndAlso _orientationSensor IsNot Nothing Then
                AddHandler _orientationSensor.OrientationChanged, AddressOf SimpleOrientationSensor_OrientationChanged
            End If
            AddHandler _displayInformation.OrientationChanged, AddressOf DisplayInformation_OrientationChanged
        End Sub
        ''' <summary>
        ''' Detects whether or not the camera is external to the device
        ''' </summary>
        Public Shared Function IsEnclosureLocationExternal(enclosureLocation As EnclosureLocation) As Boolean
            Return (enclosureLocation Is Nothing OrElse enclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Unknown)
        End Function
        ''' <summary>
        ''' Gets the rotation to rotate ui elements
        ''' </summary>
        Public Function GetUIOrientation() As SimpleOrientation
            If IsEnclosureLocationExternal(_cameraEnclosureLocation) Then
                ' Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                Return SimpleOrientation.NotRotated
            End If
            ' Return the difference between the orientation of the device and the orientation of the app display
            Dim deviceOrientation = If(_orientationSensor IsNot Nothing, _orientationSensor.GetCurrentOrientation(), SimpleOrientation.NotRotated)
            Dim displayOrientation = ConvertDisplayOrientationToSimpleOrientation(_displayInformation.CurrentOrientation)
            Return SubtractOrientations(displayOrientation, deviceOrientation)
        End Function
        ''' <summary>
        ''' Gets the rotation of the camera to rotate pictures/videos when saving to file
        ''' </summary>
        Public Function GetCameraCaptureOrientation() As SimpleOrientation
            If IsEnclosureLocationExternal(_cameraEnclosureLocation) Then
                ' Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                Return SimpleOrientation.NotRotated
            End If
            ' Get the device orientation offset by the camera hardware offset
            Dim deviceOrientation = If(_orientationSensor IsNot Nothing, _orientationSensor.GetCurrentOrientation(), SimpleOrientation.NotRotated)
            Dim result = SubtractOrientations(deviceOrientation, GetCameraOrientationRelativeToNativeOrientation())
            ' If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            If ShouldMirrorPreview() Then
                result = MirrorOrientation(result)
            End If
            Return result
        End Function
        ''' <summary>
        ''' Gets the rotation of the camera to display the camera preview
        ''' </summary>
        Public Function GetCameraPreviewOrientation() As SimpleOrientation
            If IsEnclosureLocationExternal(_cameraEnclosureLocation) Then
                ' Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                Return SimpleOrientation.NotRotated
            End If
            ' Get the app display rotation offset by the camera hardware offset
            Dim result = ConvertDisplayOrientationToSimpleOrientation(_displayInformation.CurrentOrientation)
            result = SubtractOrientations(result, GetCameraOrientationRelativeToNativeOrientation())
            ' If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            If ShouldMirrorPreview() Then
                result = MirrorOrientation(result)
            End If
            Return result
        End Function
        Public Shared Function ConvertSimpleOrientationToPhotoOrientation(orientation As SimpleOrientation) As PhotoOrientation
            Select Case orientation
                Case SimpleOrientation.Rotated90DegreesCounterclockwise
                    Return PhotoOrientation.Rotate90
                Case SimpleOrientation.Rotated180DegreesCounterclockwise
                    Return PhotoOrientation.Rotate180
                Case SimpleOrientation.Rotated270DegreesCounterclockwise
                    Return PhotoOrientation.Rotate270
                Case SimpleOrientation.NotRotated
                    Return PhotoOrientation.Normal
                Case Else
                    Return PhotoOrientation.Normal
            End Select
        End Function
        Public Shared Function ConvertSimpleOrientationToClockwiseDegrees(orientation As SimpleOrientation) As Integer
            Select Case orientation
                Case SimpleOrientation.Rotated90DegreesCounterclockwise
                    Return 270
                Case SimpleOrientation.Rotated180DegreesCounterclockwise
                    Return 180
                Case SimpleOrientation.Rotated270DegreesCounterclockwise
                    Return 90
                Case SimpleOrientation.NotRotated
                    Return 0
                Case Else
                    Return 0
            End Select
        End Function
        Private Function ConvertDisplayOrientationToSimpleOrientation(orientation As DisplayOrientations) As SimpleOrientation
            Dim result As SimpleOrientation
            Select Case orientation
                Case DisplayOrientations.Landscape
                    result = SimpleOrientation.NotRotated
                    Exit Select
                Case DisplayOrientations.PortraitFlipped
                    result = SimpleOrientation.Rotated90DegreesCounterclockwise
                    Exit Select
                Case DisplayOrientations.LandscapeFlipped
                    result = SimpleOrientation.Rotated180DegreesCounterclockwise
                    Exit Select
                Case DisplayOrientations.Portrait
                    result = SimpleOrientation.Rotated270DegreesCounterclockwise
                    Exit Select
            End Select
            ' Above assumes landscape; offset is needed if native orientation is portrait
            If _displayInformation.NativeOrientation = DisplayOrientations.Portrait Then
                result = AddOrientations(result, SimpleOrientation.Rotated90DegreesCounterclockwise)
            End If
            Return result
        End Function
        Private Shared Function MirrorOrientation(orientation As SimpleOrientation) As SimpleOrientation
            ' This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
            Select Case orientation
                Case SimpleOrientation.Rotated90DegreesCounterclockwise
                    Return SimpleOrientation.Rotated270DegreesCounterclockwise
                Case SimpleOrientation.Rotated270DegreesCounterclockwise
                    Return SimpleOrientation.Rotated90DegreesCounterclockwise
            End Select
            Return orientation
        End Function
        Private Shared Function AddOrientations(a As SimpleOrientation, b As SimpleOrientation) As SimpleOrientation
            Dim aRot = ConvertSimpleOrientationToClockwiseDegrees(a)
            Dim bRot = ConvertSimpleOrientationToClockwiseDegrees(b)
            Dim result = (aRot + bRot) Mod 360
            Return ConvertClockwiseDegreesToSimpleOrientation(result)
        End Function
        Private Shared Function SubtractOrientations(a As SimpleOrientation, b As SimpleOrientation) As SimpleOrientation
            Dim aRot = ConvertSimpleOrientationToClockwiseDegrees(a)
            Dim bRot = ConvertSimpleOrientationToClockwiseDegrees(b)
            ' Add 360 to ensure the modulus operator does not operate on a negative
            Dim result = (360 + (aRot - bRot)) Mod 360
            Return ConvertClockwiseDegreesToSimpleOrientation(result)
        End Function
        Private Shared Function ConvertSimpleOrientationToVideoRotation(orientation As SimpleOrientation) As VideoRotation
            Select Case orientation
                Case SimpleOrientation.Rotated90DegreesCounterclockwise
                    Return VideoRotation.Clockwise270Degrees
                Case SimpleOrientation.Rotated180DegreesCounterclockwise
                    Return VideoRotation.Clockwise180Degrees
                Case SimpleOrientation.Rotated270DegreesCounterclockwise
                    Return VideoRotation.Clockwise90Degrees
                Case SimpleOrientation.NotRotated
                    Return VideoRotation.None
                Case Else
                    Return VideoRotation.None
            End Select
        End Function
        Private Shared Function ConvertClockwiseDegreesToSimpleOrientation(orientation As Integer) As SimpleOrientation
            Select Case orientation
                Case 270
                    Return SimpleOrientation.Rotated90DegreesCounterclockwise
                Case 180
                    Return SimpleOrientation.Rotated180DegreesCounterclockwise
                Case 90
                    Return SimpleOrientation.Rotated270DegreesCounterclockwise
                Case 0
                    Return SimpleOrientation.NotRotated
                Case Else
                    Return SimpleOrientation.NotRotated
            End Select
        End Function
        Private Sub SimpleOrientationSensor_OrientationChanged(sender As SimpleOrientationSensor, args As SimpleOrientationSensorOrientationChangedEventArgs)
            If args.Orientation <> SimpleOrientation.Faceup AndAlso args.Orientation <> SimpleOrientation.Facedown Then
                ' Only raise the OrientationChanged event if the device is not parallel to the ground. This allows users to take pictures of documents (FaceUp)
                ' or the ceiling (FaceDown) in portrait or landscape, by first holding the device in the desired orientation, and then pointing the camera
                ' either up or down, at the desired subject.
                'Note: This assumes that the camera is either facing the same way as the screen, or the opposite way. For devices with cameras mounted
                '      on other panels, this logic should be adjusted.
                RaiseEvent OrientationChanged(False)
            End If
        End Sub
        Private Sub DisplayInformation_OrientationChanged(sender As DisplayInformation, args As Object)
            RaiseEvent OrientationChanged(True)
        End Sub
        Private Function ShouldMirrorPreview() As Boolean
            ' It is recommended that applications mirror the preview for front-facing cameras, as it gives users a more natural experience, since it behaves more like a mirror
            Return (_cameraEnclosureLocation.Panel = Windows.Devices.Enumeration.Panel.Front)
        End Function
        Private Function GetCameraOrientationRelativeToNativeOrientation() As SimpleOrientation
            ' Get the rotation angle of the camera enclosure as it is mounted in the device hardware
            Dim enclosureAngle = ConvertClockwiseDegreesToSimpleOrientation(CInt(_cameraEnclosureLocation.RotationAngleInDegreesClockwise))
            ' Account for the fact that, on portrait-first devices, the built in camera sensor is read at a 90 degree offset to the native orientation
            If _displayInformation.NativeOrientation = DisplayOrientations.Portrait AndAlso Not IsEnclosureLocationExternal(_cameraEnclosureLocation) Then
                enclosureAngle = AddOrientations(SimpleOrientation.Rotated90DegreesCounterclockwise, enclosureAngle)
            End If
            Return enclosureAngle
        End Function
    End Class
End Namespace
