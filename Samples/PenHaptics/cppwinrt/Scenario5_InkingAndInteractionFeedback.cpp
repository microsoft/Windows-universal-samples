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

#include "pch.h"
#include "SampleConfiguration.h"
#include "Scenario5_InkingAndInteractionFeedback.h"
#include "Scenario5_InkingAndInteractionFeedback.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Haptics;
using namespace Windows::Devices::Input;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Shapes;


// This sample shows how to use Inking and Interaction tactile signals in
// conjunction with each other.
// Namely, this sample plays the InkContinuous waveform when dragging a movable
// rectangle and then plays the Click waveform when the rectangle is in range
// of a snap gridline and, finally, if the movable rectangle snaps to a gridline
// upon release, it plays the Click waveform one more time.
namespace winrt::SDKTemplate::implementation
{
    // Set the initial configuration of the canvas and the rectangles
    Scenario5_InkingAndInteractionFeedback::Scenario5_InkingAndInteractionFeedback()
    {
        InitializeComponent();
        InitializeGridLines();
        InitializeManipulationTransforms();
    }

    // Draw gridlines based on the gridSize
    void Scenario5_InkingAndInteractionFeedback::InitializeGridLines()
    {
        SolidColorBrush darkGray(Colors::DarkGray());
        double width = hapticCanvas().Width();
        double height = hapticCanvas().Height();
        for (int i = 0; i <= width; i += gridSize)
        {
            Line currLine;
            currLine.Stroke(darkGray);
            currLine.X1(i);
            currLine.X2(i);
            currLine.Y1(0);
            currLine.Y2(height);
            hapticCanvas().Children().Append(currLine);
        }
        for (int i = 0; i <= height; i += gridSize)
        {
            Line currLine;
            currLine.Stroke(darkGray);
            currLine.X1(0);
            currLine.X2(width);
            currLine.Y1(i);
            currLine.Y2(i);
            hapticCanvas().Children().Append(currLine);
        }
    }

    // Configure the Manipulation transforms that are used to drag the rectangle around
    void Scenario5_InkingAndInteractionFeedback::InitializeManipulationTransforms()
    {
        previousTransform = MatrixTransform();
        previousTransform.Matrix(MatrixHelper::Identity());
        deltaTransform = CompositeTransform();
        topLeft = Point(75, 75);
        deltaTransform.TranslateX(topLeft.X);
        deltaTransform.TranslateY(topLeft.Y);

        transforms = TransformGroup();
        transforms.Children().ReplaceAll({ previousTransform, deltaTransform });

        // Set the render transform on the rect
        movableRect().RenderTransform(transforms);
    }


    // The PointerEnter event will get fired as soon as Pointer input is received in the movableRect.
    // This event handler implementation will query the device providing input to see if it's a pen and
    // then check to see the pen supports tactile feedback and, if so, configure the PenDevice
    // to send the InkContinuous tactile signal.
    void Scenario5_InkingAndInteractionFeedback::MovableRect_Entered(IInspectable const&, PointerRoutedEventArgs const& e)
    {
        // If the current Pointer device is not a pen, exit
        if (e.Pointer().PointerDeviceType() != PointerDeviceType::Pen)
        {
            return;
        }

        // Attempt to retrieve the PenDevice from the current PointerId
        penDevice = PenDevice::GetFromPointerId(e.Pointer().PointerId());

        // If a PenDevice cannot be retrieved based on the PointerId, it does not support
        // advanced pen features, such as tactile feedback
        if (penDevice == nullptr)
        {
            statusText().Text(L"Advanced pen features not supported");
            return;
        }

        // Check to see if the current PenDevice supports tactile feedback by seeing if it
        // has a SimpleHapticsController
        hapticsController = penDevice.SimpleHapticsController();
        if (hapticsController == nullptr)
        {
            statusText().Text(L"This pen does not provide tactile feedback");
            return;
        }

        // Send the InkContinuous waveform to the PenDevice's SimpleHapticsController.
        // Once sent, inking continuous tactile feedback will be triggered as soon as the pen tip touches
        // the screen and will be stopped once the pen tip is lifted from the screen.
        // Also, check to see if the current PenDevice's SimpleHapticsController supports
        // setting the intensity value of the tactile feedback.  If so, set it to 0.75.
        // If not, send the waveform with the default intensity.
        SimpleHapticsControllerFeedback feedback = FindSupportedFeedback(hapticsController, KnownSimpleHapticsControllerWaveforms::InkContinuous());
        if (feedback != nullptr)
        {
            if (hapticsController.IsIntensitySupported())
            {
                hapticsController.SendHapticFeedback(feedback, 0.75);
            }
            else
            {
                hapticsController.SendHapticFeedback(feedback);
            }
        }
    }

    // Stop sending the tactile feedback and clear the current penDevice and hapticsController on PointerExit.
    // Stopping the feedback is important as it clears the tactile signal from the PenDevice's
    // SimpleHapticsController, ensuring that it has a clean state once it next enters range.
    void Scenario5_InkingAndInteractionFeedback::MovableRect_Exited(IInspectable const&, PointerRoutedEventArgs const&)
    {
        penDevice = nullptr;
        statusText().Text(L"");
        if (hapticsController != nullptr)
        {
            hapticsController.StopFeedback();
            hapticsController = nullptr;
        }
    }

    // Change the color of the movableRect once a manipulation is started. This coincides with the pen
    // touching down inside the rectangle, so this color change will happen at the same time that the
    // InkContinuous waveform begins playing.
    void Scenario5_InkingAndInteractionFeedback::MovableRect_ManipulationStarted(IInspectable const&, ManipulationStartedRoutedEventArgs const&)
    {
        movableRect().Style(MovableRectangleDragged());
    }

    // Update the position of the movableRect and update the previewRect as needed
    void Scenario5_InkingAndInteractionFeedback::MovableRect_ManipulationDelta(IInspectable const&, ManipulationDeltaRoutedEventArgs const& e)
    {
        previousTransform.Matrix(transforms.Value());

        // Look at the Delta property of the ManipulationDeltaRoutedEventArgs to retrieve the X and Y changes
        Point translation = e.Delta().Translation;
        deltaTransform.TranslateX(translation.X);
        deltaTransform.TranslateY(translation.Y);
        topLeft.X += translation.X;
        topLeft.Y += translation.Y;
        UpdatePreviewRect();
    }

    // Update the visibilty or position of the previewRect based on the location
    // of the movableRect.
    // If the previewRect is snapped to a given gridline for the first time,
    // play the Click tactile signal.
    void Scenario5_InkingAndInteractionFeedback::UpdatePreviewRect()
    {
        if (IsInSnapRange())
        {
            previewRect().Visibility(Visibility::Visible);
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
                previewRect().Translation({ previewTopLeft.X, previewTopLeft.Y, 0 });
                if (hapticsController != nullptr && sendHaptics)
                {
                    SimpleHapticsControllerFeedback feedback = FindSupportedFeedback(hapticsController, KnownSimpleHapticsControllerWaveforms::Click());
                    if (feedback != nullptr)
                    {
                        hapticsController.SendHapticFeedback(feedback);
                    }
                }
            }
        }
        else
        {
            previewRect().Visibility(Visibility::Collapsed);
            previewTopLeft = Point(-1, -1);
        }
    }

    // When the manipulation is over, reset the color of the movableRect and
    // snap it to a gridline if appropriate
    void Scenario5_InkingAndInteractionFeedback::MovableRect_ManipulationCompleted(IInspectable const&, ManipulationCompletedRoutedEventArgs const&)
    {
        movableRect().Style(MovableRectangle());
        if (IsInSnapRange())
        {
            SnapMovableRectToGridLines();
        }
    }

    // Updates the position of the movableRect to snap to the appropriate gridline(s).
    // In addition, play the Click tactile signal to provide additional feedback to the user that it has snapped.
    void Scenario5_InkingAndInteractionFeedback::SnapMovableRectToGridLines()
    {
        topLeft = GetSnappedPoint();
        previousTransform = MatrixTransform();
        previousTransform.Matrix(MatrixHelper::Identity());
        deltaTransform.TranslateX(topLeft.X);
        deltaTransform.TranslateY(topLeft.Y);

        transforms = TransformGroup();
        transforms.Children().ReplaceAll({ previousTransform, deltaTransform });

        movableRect().RenderTransform(transforms);

        if (hapticsController != nullptr)
        {
            SimpleHapticsControllerFeedback feedback = FindSupportedFeedback(hapticsController, KnownSimpleHapticsControllerWaveforms::Click());
            if (feedback != nullptr)
            {
                hapticsController.SendHapticFeedback(feedback);
            }
        }
    }

    // Retrieve the point corresponding to the top-left corner of where the movableRect would snap to
    // if released.
    Point Scenario5_InkingAndInteractionFeedback::GetSnappedPoint()
    {
        Point snappedPoint;
        float remainderX = (float)fmod(topLeft.X, gridSize);
        if (remainderX <= snapDistance)
        {
            snappedPoint.X = topLeft.X - remainderX;
        }
        else if (remainderX >= gridSize - snapDistance)
        {
            snappedPoint.X = topLeft.X + gridSize - remainderX;
        }
        else
        {
            snappedPoint.X = topLeft.X;
        }
        float remainderY = (float)fmod(topLeft.Y, gridSize);
        if (remainderY <= snapDistance)
        {
            snappedPoint.Y = topLeft.Y - remainderY;
        }
        else if (remainderY >= gridSize - snapDistance)
        {
            snappedPoint.Y = topLeft.Y + gridSize - remainderY;
        }
        else
        {
            snappedPoint.Y = topLeft.Y;
        }
        return snappedPoint;
    }

    // Check to see if the movableRect is currently within snapping range of a gridline
    bool Scenario5_InkingAndInteractionFeedback::IsInSnapRange()
    {
        float remainderX = (float)fmod(topLeft.X, gridSize);
        float remainderY = (float)fmod(topLeft.Y, gridSize);
        return remainderX <= snapDistance ||
            remainderX >= gridSize - snapDistance ||
            remainderY <= snapDistance ||
            remainderY >= gridSize - snapDistance;
    }
}
