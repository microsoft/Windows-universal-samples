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
#include "SkeletalFrameRenderer.h"
#include <initguid.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Shapes;
using namespace WindowsPreview::Media::Capture::Frames;

// The Templated Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234235

// Skeleton colors.
static std::array<Color, 9> colors = {
    ColorHelper::FromArgb(0xFF, 0x7F, 0x00, 0x00),
    ColorHelper::FromArgb(0xFF, 0xFF, 0x00, 0x00),
    ColorHelper::FromArgb(0xFF, 0xFF, 0x7F, 0x00),
    ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0x00),
    ColorHelper::FromArgb(0xFF, 0x7F, 0xFF, 0x7F),
    ColorHelper::FromArgb(0xFF, 0x00, 0xFF, 0xFF),
    ColorHelper::FromArgb(0xFF, 0x00, 0x7F, 0xFF),
    ColorHelper::FromArgb(0xFF, 0x00, 0x00, 0xFF),
    ColorHelper::FromArgb(0xFF, 0x00, 0x00, 0x7F)
};

// Returns line between two body parts transformed using supplied transformation function.
static Line^ LineTo(
    Array<TrackedPose>^ poses,
    BodyPart fromBodyPart,
    BodyPart toBodyPart,
    CoordinateTransformationMethod^ positionTransformation)
{
    TrackedPose fromTrackedBodyPart = poses[static_cast<UINT32>(fromBodyPart)];
    TrackedPose toTrackedBodypart = poses[static_cast<UINT32>(toBodyPart)];

    // Only render tracked bones.
    if (fromTrackedBodyPart.PositionStatus == TrackingStatus::NotTracked ||
        fromTrackedBodyPart.PositionStatus == TrackingStatus::Inferred ||
        toTrackedBodypart.PositionStatus == TrackingStatus::NotTracked ||
        toTrackedBodypart.PositionStatus == TrackingStatus::Inferred)
    {
        return nullptr;
    }

    Point fromPosition = positionTransformation(fromTrackedBodyPart.Position);
    Point toPosition = positionTransformation(toTrackedBodypart.Position);

    Line^ line = ref new Line();

    line->X1 = fromPosition.X;
    line->Y1 = fromPosition.Y;
    line->X2 = toPosition.X;
    line->Y2 = toPosition.Y;

    return line;
}

static std::vector<Shape^> RenderSkeletonShapes(
    PoseTrackingEntity^ poseEntity, 
    Brush^ shapeColor, 
    CoordinateTransformationMethod^ positionTransformation)
{
    if (poseEntity == nullptr || !poseEntity->IsTracked)
    {
        return std::vector<Shape^>();
    }
    
    Array<TrackedPose>^ poses = ref new Array<TrackedPose>(poseEntity->PosesCount);
    poseEntity->GetPoses(poses);
    
    // Create line shapes for all bones in the skeleton
    std::vector<Shape^> shapes{
        LineTo(poses, BodyPart::Head,         BodyPart::Neck,            positionTransformation),
        LineTo(poses, BodyPart::Neck,         BodyPart::SpineShoulder,   positionTransformation),
        LineTo(poses, BodyPart::SpineShoulder,BodyPart::ShoulderLeft,    positionTransformation),
        LineTo(poses, BodyPart::SpineShoulder,BodyPart::ShoulderRight,   positionTransformation),
        LineTo(poses, BodyPart::ShoulderLeft, BodyPart::ElbowLeft,       positionTransformation),
        LineTo(poses, BodyPart::ShoulderRight,BodyPart::ElbowRight,      positionTransformation),
        LineTo(poses, BodyPart::ElbowLeft,    BodyPart::WristLeft,       positionTransformation),
        LineTo(poses, BodyPart::ElbowRight,   BodyPart::WristRight,      positionTransformation),
        LineTo(poses, BodyPart::WristLeft,    BodyPart::HandLeft,        positionTransformation),
        LineTo(poses, BodyPart::WristRight,   BodyPart::HandRight,       positionTransformation),
        LineTo(poses, BodyPart::HandLeft,     BodyPart::HandTipLeft,     positionTransformation),
        LineTo(poses, BodyPart::HandRight,    BodyPart::HandTipRight,    positionTransformation),
        LineTo(poses, BodyPart::HandLeft,     BodyPart::ThumbLeft,       positionTransformation),
        LineTo(poses, BodyPart::HandRight,    BodyPart::ThumbRight,      positionTransformation),
        LineTo(poses, BodyPart::SpineShoulder,BodyPart::SpineMid,        positionTransformation),
        LineTo(poses, BodyPart::SpineMid,     BodyPart::SpineBase,       positionTransformation),
        LineTo(poses, BodyPart::SpineBase,    BodyPart::HipLeft,         positionTransformation),
        LineTo(poses, BodyPart::SpineBase,    BodyPart::HipRight,        positionTransformation),
        LineTo(poses, BodyPart::HipLeft,      BodyPart::KneeLeft,        positionTransformation),
        LineTo(poses, BodyPart::HipRight,     BodyPart::KneeRight,       positionTransformation),
        LineTo(poses, BodyPart::KneeLeft,     BodyPart::AnkleLeft,       positionTransformation),
        LineTo(poses, BodyPart::KneeRight,    BodyPart::AnkleRight,      positionTransformation),
        LineTo(poses, BodyPart::AnkleLeft,    BodyPart::FootLeft,        positionTransformation),
        LineTo(poses, BodyPart::AnkleRight,   BodyPart::FootRight,       positionTransformation)
    };

    // Apply shape color style.
    for (Shape^ const& boneShape : shapes)
    {
        if (boneShape)
        {
            boneShape->StrokeThickness = 5;
            boneShape->Stroke = shapeColor;
        }
    }

    return shapes;
}

SkeletalFrameRenderer::SkeletalFrameRenderer() :
    Canvas()
{
    for(auto const& color : colors)
    {
        m_brushes.push_back(ref new SolidColorBrush(color));
    }
}

void SkeletalFrameRenderer::Clear()
{
    Children->Clear();
}

void SkeletalFrameRenderer::Render(PoseTrackingFrame^ frame, CoordinateTransformationMethod^ positionTransformation)
{
    Clear();

    for (UINT32 i = 0; i < frame->Entities->Size; i++)
    {
        PoseTrackingEntity^ poseEntity = frame->Entities->GetAt(i);
        
        if (poseEntity == nullptr ||
            !poseEntity->IsTracked ||
            !poseEntity->PoseSetId.Equals(KnownPoseTrackingPointSets::BodyTrackingPointSet))
        {
            continue;
        }

        Brush^ lineColor = m_brushes[i % m_brushes.size()];

        for (UIElement^ const& bone : RenderSkeletonShapes(poseEntity, lineColor, positionTransformation))
        {
            if (bone)
            {
                Children->Append(bone);
            }
        }
    }
}