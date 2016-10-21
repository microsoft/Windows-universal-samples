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

#pragma once

namespace SDKTemplate
{
    // Delegate used to map skeletal points from depth space to required display space.
    public delegate Windows::Foundation::Point CoordinateTransformationMethod(Windows::Foundation::Numerics::float3);

    public ref class SkeletalFrameRenderer sealed : public Windows::UI::Xaml::Controls::Canvas
    {
    public:
        SkeletalFrameRenderer();

        /// <summary>
        /// Clears all rendered shapes on this canvas.
        /// </summary>
        void Clear();

        /// <summary>
        /// Renders a pose tracking frame to this canvas.
        /// </summary>
        void Render(WindowsPreview::Media::Capture::Frames::PoseTrackingFrame^ frame, CoordinateTransformationMethod^ transformation);

    private:
        std::vector<Windows::UI::Xaml::Media::Brush^> m_brushes;

        static Windows::UI::Xaml::Shapes::Line^ LineTo(
            Platform::Array<WindowsPreview::Media::Capture::Frames::TrackedPose>^ poses,
            WindowsPreview::Media::Capture::Frames::BodyPart fromBodyPart,
            WindowsPreview::Media::Capture::Frames::BodyPart toBodyPart,
            CoordinateTransformationMethod^ positionTransformation);
    };
} // CameraStreamCorrelation