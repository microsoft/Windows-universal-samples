//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************


using System.Numerics;
using Windows.Devices.Haptics;
using Windows.Devices.Input;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Shapes;

// This sample shows how to use Inking and Interaction tactile signals in
// conjunction with each other.
// Namely, this sample plays the InkContinuous waveform when dragging a movable
// rectangle and then plays the Click waveform when the rectangle is in range
// of a snap gridline and, finally, if the movable rectangle snaps to a gridline
// upon release, it plays the Click waveform one more time.
namespace SDKTemplate
{
    public sealed partial class Scenario5_InkingAndInteractionFeedback : Page
    {
        private readonly int gridSize = 100;
        readonly double snapDistance = 20;
        private Point topLeft;
        private Point previewTopLeft = new Point(-1, -1);
        private TransformGroup transforms;
        private MatrixTransform previousTransform;
        private CompositeTransform deltaTransform;
        PenDevice penDevice;
        SimpleHapticsController hapticsController;

        // Set the initial configuration of the canvas and the rectangles
        public Scenario5_InkingAndInteractionFeedback()
        {
            InitializeComponent();
            InitializeGridLines();
            InitializeManipulationTransforms();
        }

        // Draw gridlines based on the gridSize
        private void InitializeGridLines()
        {
            SolidColorBrush darkGray = new SolidColorBrush(Colors.DarkGray);
            double width = hapticCanvas.Width;
            double height = hapticCanvas.Height;
            for (int i = 0; i <= width; i += gridSize)
            {
                Line currLine = new Line() {
                    Stroke = darkGray,
                    X1 = i,
                    X2 = i,
                    Y1 = 0,
                    Y2 = height,
                };
                hapticCanvas.Children.Add(currLine);
            }
            for (int i = 0; i <= height; i += gridSize)
            {
                Line currLine = new Line() {
                    Stroke = darkGray,
                    X1 = 0,
                    X2 = width,
                    Y1 = i,
                    Y2 = i,
                };
                hapticCanvas.Children.Add(currLine);
            }
        }

        // Configure the Manipulation transforms that are used to drag the rectangle around
        private void InitializeManipulationTransforms()
        {
            previousTransform = new MatrixTransform() { Matrix = Matrix.Identity };
            topLeft = new Point(75, 75);
            deltaTransform = new CompositeTransform() { TranslateX = topLeft.X, TranslateY = topLeft.Y };

            transforms = new TransformGroup() { Children = { previousTransform, deltaTransform } };

            // Set the render transform on the rect
            movableRect.RenderTransform = transforms;
        }

        // The PointerEnter event will get fired as soon as Pointer input is received in the movableRect.
        // This event handler implementation will query the device providing input to see if it's a pen and
        // then check to see the pen supports tactile feedback and, if so, configure the PenDevice
        // to send the InkContinuous tactile signal.
        private void MovableRect_Entered(object sender, PointerRoutedEventArgs e)
        {
            // If the current Pointer device is not a pen, exit
            if (e.Pointer.PointerDeviceType != PointerDeviceType.Pen)
            {
                return;
            }

            // Attempt to retrieve the PenDevice from the current PointerId
            penDevice = PenDevice.GetFromPointerId(e.Pointer.PointerId);

            // If a PenDevice cannot be retrieved based on the PointerId, it does not support
            // advanced pen features, such as tactile feedback
            if (penDevice == null)
            {
                statusText.Text = "Advanced pen features not supported";
                return;
            }

            // Check to see if the current PenDevice supports tactile feedback by seeing if it
            // has a SimpleHapticsController
            hapticsController = penDevice.SimpleHapticsController;
            if (hapticsController == null)
            {
                statusText.Text = "This pen does not provide tactile feedback";
                return;
            }

            // Send the InkContinuous waveform to the PenDevice's SimpleHapticsController.
            // Once sent, inking continuous tactile feedback will be triggered as soon as the pen tip touches
            // the screen and will be stopped once the pen tip is lifted from the screen.
            // Also, check to see if the current PenDevice's SimpleHapticsController supports
            // setting the intensity value of the tactile feedback.  If so, set it to 0.75.
            // If not, send the waveform with the default intensity.
            foreach (var waveform in hapticsController.SupportedFeedback)
            {
                if (waveform.Waveform == KnownSimpleHapticsControllerWaveforms.InkContinuous)
                {
                    if (hapticsController.IsIntensitySupported)
                    {
                        hapticsController.SendHapticFeedback(waveform, 0.75);
                     }
                    else
                    {
                        hapticsController.SendHapticFeedback(waveform);
                    }
                }
            }

        }

        // Stop sending the tactile feedback and clear the current penDevice and hapticsController on PointerExit.
        // Stopping the feedback is important as it clears the tactile signal from the PenDevice's
        // SimpleHapticsController, ensuring that it has a clean state once it next enters range.
        private void MovableRect_Exited(object sender, PointerRoutedEventArgs e)
        {
            penDevice = null;
            statusText.Text = "";
            if (hapticsController != null)
            {
                hapticsController.StopFeedback();
                hapticsController = null;
            }
        }

        // Change the color of the movableRect once a manipulation is started. This coincides with the pen
        // touching down inside the rectangle, so this color change will happen at the same time that the
        // InkContinuous waveform begins playing.
        private void MovableRect_ManipulationStarted(object sender, ManipulationStartedRoutedEventArgs e)
        {
            movableRect.Style = MovableRectangleDragged;
        }

        // Update the position of the movableRect and update the previewRect as needed
        private void MovableRect_ManipulationDelta(object sender, ManipulationDeltaRoutedEventArgs e)
        {
            previousTransform.Matrix = transforms.Value;

            // Look at the Delta property of the ManipulationDeltaRoutedEventArgs to retrieve the X and Y changes
            deltaTransform.TranslateX = e.Delta.Translation.X;
            deltaTransform.TranslateY = e.Delta.Translation.Y;
            topLeft.X += e.Delta.Translation.X;
            topLeft.Y += e.Delta.Translation.Y;
            UpdatePreviewRect();
        }

        // Update the visibilty or position of the previewRect based on the location
        // of the movableRect.
        // If the previewRect is snapped to a given gridline for the first time,
        // play the Click tactile signal.
        private void UpdatePreviewRect()
        {
            if (IsInSnapRange())
            {
                previewRect.Visibility = Visibility.Visible;
                Point newPreviewTopLeft = GetSnappedPoint();

                // Check to see if this is the first time that the previewRect is in range of a given gridline
                // and send the Click tactile signal if so
                bool sendHaptics = true;
                if ((previewTopLeft.X == newPreviewTopLeft.X && previewTopLeft.Y == newPreviewTopLeft.Y) ||
                    (previewTopLeft.X == newPreviewTopLeft.X && newPreviewTopLeft.Y == topLeft.Y) ||
                    (previewTopLeft.Y == newPreviewTopLeft.Y && newPreviewTopLeft.X == topLeft.X))
                {
                    sendHaptics = false;
                }
                if (previewTopLeft.X != newPreviewTopLeft.X || previewTopLeft.Y != newPreviewTopLeft.Y)
                {
                    previewTopLeft = newPreviewTopLeft;
                    previewRect.Translation = new Vector3((float)previewTopLeft.X, (float)previewTopLeft.Y, 0);
                    if (hapticsController != null && sendHaptics)
                    {
                        foreach (var waveform in hapticsController.SupportedFeedback)
                        {
                            if (waveform.Waveform == KnownSimpleHapticsControllerWaveforms.Click)
                            {
                                hapticsController.SendHapticFeedback(waveform);
                            }
                        }
                    }
                }
            }
            else
            {
                previewRect.Visibility = Visibility.Collapsed;
                previewTopLeft = new Point(-1, -1);
            }
        }

        // When the manipulation is over, reset the color of the movableRect and
        // snap it to a gridline if appropriate
        private void MovableRect_ManipulationCompleted(object sender, ManipulationCompletedRoutedEventArgs e)
        {
            movableRect.Style = MovableRectangle;
            if (IsInSnapRange())
            {
                SnapMovableRectToGridLines();
            }
        }
        
        // Updates the position of the movableRect to snap to the appropriate gridline(s).
        // In addition, play the Click tactile signal to provide additional feedback to the user that it has snapped.
        private void SnapMovableRectToGridLines()
        {
            topLeft = GetSnappedPoint();
            previousTransform = new MatrixTransform() { Matrix = Matrix.Identity };
            deltaTransform.TranslateX = topLeft.X;
            deltaTransform.TranslateY = topLeft.Y;

            transforms = new TransformGroup() { Children = { previousTransform, deltaTransform } };

            movableRect.RenderTransform = transforms;

            if (hapticsController != null)
            {
                foreach (var waveform in hapticsController.SupportedFeedback)
                {
                    if (waveform.Waveform == KnownSimpleHapticsControllerWaveforms.Click)
                    {
                        hapticsController.SendHapticFeedback(waveform);
                    }
                }
            }
        }

        // Retrieve the point corresponding to the top-left corner of where the movableRect would snap to
        // if released.
        private Point GetSnappedPoint()
        {
            Point snappedPoint;
            if (topLeft.X % gridSize <= snapDistance)
            {
                snappedPoint.X = topLeft.X + topLeft.X % gridSize * -1;
            }
            else if (topLeft.X % gridSize >= gridSize - snapDistance)
            {
                snappedPoint.X = topLeft.X + gridSize - (topLeft.X % gridSize);
            }
            else
            {
                snappedPoint.X = topLeft.X;
            }
            if (topLeft.Y % gridSize <= snapDistance)
            {
                snappedPoint.Y = topLeft.Y + topLeft.Y % gridSize * -1;
            }
            else if (topLeft.Y % gridSize >= gridSize - snapDistance)
            {
                snappedPoint.Y = topLeft.Y + gridSize - (topLeft.Y % gridSize);
            }
            else
            {
                snappedPoint.Y = topLeft.Y;
            }
            return snappedPoint;
        }

        // Check to see if the movableRect is currently within snapping range of a gridline
        private bool IsInSnapRange()
        {
            return topLeft.X % gridSize <= snapDistance ||
                topLeft.X % gridSize >= gridSize - snapDistance ||
                topLeft.Y % gridSize <= snapDistance ||
                topLeft.Y % gridSize >= gridSize - snapDistance;
        }
    }
}
