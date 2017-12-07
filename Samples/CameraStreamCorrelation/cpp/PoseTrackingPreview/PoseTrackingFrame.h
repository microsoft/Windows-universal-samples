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
#include "MFPoseTrackingFrame.h"

namespace WindowsPreview {
    namespace Media {
        namespace Capture {
            namespace Frames {

                public enum class TrackingStatus : int32_t {
                    NotTracked = static_cast<int32_t>(MFPoseTrackingPreview::TrackingStatus::NotTracked),
                    Inferred = static_cast<int32_t>(MFPoseTrackingPreview::TrackingStatus::Inferred),
                    Tracked = static_cast<int32_t>(MFPoseTrackingPreview::TrackingStatus::Tracked)
                };

                public value struct TrackedPose {
                    TrackingStatus PositionStatus;
                    Windows::Foundation::Numerics::float3 Position;
                    TrackingStatus OrientationStatus;
                    Windows::Foundation::Numerics::quaternion Orientation;
                };

                // When PoseSet field of PoseTrackingEntity is set to PoseSet_BodyTracking, the
                // Poses of the PoseTrackingEntity follow the order defined by this enum
                public enum class BodyPart : int32_t {
                    SpineBase     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::SpineBase),
                    SpineMid      = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::SpineMid),
                    Neck          = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::Neck),
                    Head          = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::Head),
                    ShoulderLeft  = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::ShoulderLeft),
                    ElbowLeft     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::ElbowLeft),
                    WristLeft     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::WristLeft),
                    HandLeft      = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::HandLeft),
                    ShoulderRight = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::ShoulderRight),
                    ElbowRight    = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::ElbowRight),
                    WristRight    = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::WristRight),
                    HandRight     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::HandRight),
                    HipLeft       = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::HipLeft),
                    KneeLeft      = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::KneeLeft),
                    AnkleLeft     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::AnkleLeft),
                    FootLeft      = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::FootLeft),
                    HipRight      = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::HipRight),
                    KneeRight     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::KneeRight),
                    AnkleRight    = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::AnkleRight),
                    FootRight     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::FootRight),
                    SpineShoulder = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::SpineShoulder),
                    HandTipLeft   = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::HandTipLeft),
                    ThumbLeft     = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::ThumbLeft),
                    HandTipRight  = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::HandTipRight),
                    ThumbRight    = static_cast<int32_t>(MFPoseTrackingPreview::BodyPart::ThumbRight)
                };

                public ref class KnownPoseTrackingPointSets sealed
                {
                public:
                    static property Platform::Guid BodyTrackingPointSet
                    {
                        Platform::Guid get() { return MFPoseTrackingPreview::PoseSet_BodyTracking; }
                    }

                    static property Platform::Guid HandTrackingPointSet
                    {
                        Platform::Guid get() { return MFPoseTrackingPreview::PoseSet_HandTracking; }
                    }

                };

                public ref class PoseTrackingEntity sealed
                {
                public:
                    property Platform::Guid EntityId
                    {
                        Platform::Guid get() { return _entityId; }
                    }
                    property Platform::Guid PoseSetId
                    {
                        Platform::Guid get() { return _poseSetId; }
                    }
                    property unsigned int PosesCount
                    {
                        unsigned int get() { return _posesCount; }
                    }
                    property bool IsTracked
                    {
                        bool get() { return _isTracked; }
                    }

                    // Fills the poses array parameter with the Poses contained in the entity
                    // The array must be large enough, i.e. contain at least PosesCount elements
                    // Exactly PosesCount elements are written; the remaining of the array is unchanged
                    void GetPoses(_Out_ Platform::WriteOnlyArray<TrackedPose>^ poses);

                    // Fills the poses array parameter with the Poses contained in the entity
                    // and at the positions indicated by the indices parameter
                    // poses array must be at least as large as indices array
                    // Exactly indices->Length elements are written; the rest of the array is unchanged
                    void GetPoses(_In_ const Platform::Array<unsigned int>^ indices, _Out_ Platform::WriteOnlyArray<TrackedPose>^ poses);

                    // Retrieves the location of Provider specific entity data in the frame buffer
                    BufferView^ GetCustomData();

                internal:
                    PoseTrackingEntity(_In_ Windows::Storage::Streams::IBuffer^ frameBuffer, _In_ unsigned int entityIndex, _In_ const MFPoseTrackingPreview::PoseTrackingEntityData* entityHeader);

                private:
                    // Cached values for frequently access data
                    Platform::Guid _entityId;
                    Platform::Guid _poseSetId;
                    unsigned int _posesCount;
                    size_t _dataSize;
                    bool _isTracked;

                    // Access to the full buffer
                    Windows::Storage::Streams::IBuffer^ _frameBuffer;
                    unsigned int _entityIndex;
                };

                ref class PoseTrackingFrame;

                public enum class PoseTrackingFrameCreationStatus
                {
                    Success, // The buffer could be correctly accessed and interpreted as a Pose Tracking Frame
                    NotAPerceptionMediaType, // The Media Type associated to the MediaFrameReference was not Perception
                    NotAPoseTrackingFrame,  // The Media Subtype associated to the MediaFrameReference was not Pose Tracking
                    FrameBufferReleased, // The Buffer associated to the MediaFrameReference has already been already released
                    InvalidFormat // There was a mistmatch between expected and real value of some internal fields of the buffer
                };

                public ref class PoseTrackingFrameCreationResult sealed
                {
                public:
                    property PoseTrackingFrame^ Frame
                    {
                        PoseTrackingFrame^ get() { return _frame; }
                    }
                    property PoseTrackingFrameCreationStatus Status
                    {
                        PoseTrackingFrameCreationStatus get() { return _status; }
                    }
                internal:
                    PoseTrackingFrameCreationResult(_In_opt_ PoseTrackingFrame^ frame, _In_ PoseTrackingFrameCreationStatus status)
                        : _frame(frame), _status(status)
                    {
                    }
                private:
                    PoseTrackingFrame^ _frame;
                    PoseTrackingFrameCreationStatus _status;
                };

                public ref class PoseTrackingFrame sealed
                {
                public:
                    static property Platform::Guid PoseTrackingSubtype
                    {
                        Platform::Guid get()
                        {
                            return MFPoseTrackingPreview::MFPerceptionFormat_PoseTracking;
                        }
                    }

                    property Windows::Media::Capture::Frames::MediaFrameReference^ MediaFrameReference
                    {
                        Windows::Media::Capture::Frames::MediaFrameReference^ get() { return _mediaFrameReference; }
                    }

                    property Windows::Foundation::Collections::IVectorView<PoseTrackingEntity^>^ Entities
                    {
                        Windows::Foundation::Collections::IVectorView<PoseTrackingEntity^>^ get() { return _entities; }
                    }

                    property Platform::Guid CustomDataType
                    {
                        Platform::Guid get() { return _customDataType; }
                    }

                    // Creates a PoseTrackingFrame from the MediaFrameReference
                    // If the MediaFrameReference does not contain Perception / PoseTracking data, the returned value is nullptr
                    static PoseTrackingFrameCreationResult^ Create(_In_ Windows::Media::Capture::Frames::MediaFrameReference^ mediaFrameReference);

                    // Retrieves the location of Provider specific frame data in the frame buffer
                    BufferView^ GetCustomData();

                private:
                    Windows::Media::Capture::Frames::MediaFrameReference^ _mediaFrameReference;
                    Windows::Storage::Streams::IBuffer^ _frameBuffer;
                    Windows::Foundation::Collections::IVectorView<PoseTrackingEntity^>^ _entities;
                    Platform::Guid _customDataType;

                    PoseTrackingFrame(Windows::Media::Capture::Frames::MediaFrameReference^ mediaFrameReference,
                        _In_ REFIID customDataType,
                        _In_ Windows::Storage::Streams::IBuffer^ frameBuffer,
                        _In_ Windows::Foundation::Collections::IVectorView<PoseTrackingEntity^>^ entities);
                };
            }
        }
    }
}
