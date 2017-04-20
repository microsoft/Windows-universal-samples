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

// Namespace and types to parse MediaFoundation perception buffer payload.
namespace MFPoseTrackingPreview
{

    enum class TrackingStatus : int32_t {
        NotTracked = 0,
        Inferred = 1,
        Tracked = 2,
    };

    // When PoseSet field of PoseTrackingEntity is set to PoseSet_BodyTracking, the
    // Poses of the PoseTrackingEntity follow the order defined by this enum
    enum class BodyPart : int32_t {
        SpineBase = 0,
        SpineMid = 1,
        Neck = 2,
        Head = 3,
        ShoulderLeft = 4,
        ElbowLeft = 5,
        WristLeft = 6,
        HandLeft = 7,
        ShoulderRight = 8,
        ElbowRight = 9,
        WristRight = 10,
        HandRight = 11,
        HipLeft = 12,
        KneeLeft = 13,
        AnkleLeft = 14,
        FootLeft = 15,
        HipRight = 16,
        KneeRight = 17,
        AnkleRight = 18,
        FootRight = 19,
        SpineShoulder = 20,
        HandTipLeft = 21,
        ThumbLeft = 22,
        HandTipRight = 23,
        ThumbRight = 24,
    };

    struct TrackedPose {
        TrackingStatus PositionStatus;
        Windows::Foundation::Numerics::float3 Position;
        TrackingStatus OrientationStatus;
        Windows::Foundation::Numerics::quaternion Orientation;
    };

    // The layout of a pose frame buffer is:
    //   1. PoseTrackingFrameHeader: Common structure for all providers.
    //      1. CustomDataTypeGuid: Guid corresponding to specific provider (16 bytes).
    //      2. EntitiesCount: How many entities are in this frame (4 bytes).
    //      3. EntityOffsets: Offsets of entity data from buffer start (4 * EntitiesCount bytes).
    //
    //   2. Provider-specific, per-frame data.
    //
    //   3. PoseTrackingEntityData #0: First entity data. Common structure for all providers.
    //      1. DataSizeInBytes: Size of entire entity (PoseTrackingEntityData + custom data) in bytes (4 bytes).
    //      2. EntityId: Allows correlation between frames (16 bytes).
    //      3. PoseSet: Guids for Body (such as Kinect), Handtracking, etc.: defines the meaning of each Pose (16 bytes).
    //      4. PosesCount: Count of poses in this entity data (4 bytes).
    //      5. IsTracked: Whether or not this entity is being tracked (1 byte).
    //      6. Poses: Array of common structure TrackedPose (sizeof(TrackedPose) * PosesCount bytes).
    //      7. Customer specific data for this entity (DataSizeInBytes - sizeof(PoseTrackedEntityData) - sizeof(TrackedPose) * (PosesCount -1) bytes)
    //
    //   4. Provider-specific data for entity #0 in this frame.
    //
    //   5. PoseTrackingEntityData #1: Second entity data. Common structure for all providers.
    //
    //   6. Provider-specific data for entity #1 in this frame.

    struct PoseTrackingFrameHeader
    {
        // This Guid allows the consumer of the buffer to verify that
        // it has been written by the expected provider
        GUID CustomDataTypeGuid;
        uint32_t EntitiesCount;
        uint32_t EntityOffsets[1]; // actual length is EntitiesCount
    };

    struct PoseTrackingEntityData
    {
        uint32_t DataSizeInBytes;
        GUID EntityId;
        GUID PoseSet;
        uint32_t PosesCount;
        bool IsTracked;
        TrackedPose Poses[1]; // actual length is PosesCount
    };

    // Subtype for PoseTracking
    // {69232056-2ED9-4D0E-89CC-5D2734A56808}
    DEFINE_GUID(MFPerceptionFormat_PoseTracking, 0x69232056, 0x2ed9, 0x4d0e, 0x89, 0xcc, 0x5d, 0x27, 0x34, 0xa5, 0x68, 0x8);

    // These two constants (used in the PoseSet attribute of PoseTrackingEntityData) define which
    // body part each TrackedPose corresponds to.
    // i.e. for body tracking: indexes in Poses correspond to values of the BodyPart enumerated type.
    //      for hand tracking: enumerated type is not yet defined.
    // {84520B1F-AB61-46DA-AB1D-E01340EF884E}
    DEFINE_GUID(PoseSet_BodyTracking, 0x84520b1f, 0xab61, 0x46da, 0xab, 0x1d, 0xe0, 0x13, 0x40, 0xef, 0x88, 0x4e);
    // {F142C82C-3A57-4E7D-8159-98BDBD6CCFE2}
    DEFINE_GUID(PoseSet_HandTracking, 0xf142c82c, 0x3a57, 0x4e7d, 0x81, 0x59, 0x98, 0xbd, 0xbd, 0x6c, 0xcf, 0xe2);

}
