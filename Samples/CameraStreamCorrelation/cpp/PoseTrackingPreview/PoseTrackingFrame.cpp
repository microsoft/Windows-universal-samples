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
#include <initguid.h>
#include "PoseTrackingFrame.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace WindowsPreview::Media::Capture::Frames;

static const wchar_t *UnknownMediaType= L"Unknown";
static const wchar_t *PerceptionMediaType= L"Perception";

// Formatted Guid strings are 38 characters long, not including the terminator
const unsigned int GuidBufferLength = 40;
static wchar_t PoseTrackingFormat[GuidBufferLength] = { 0 };
static wchar_t PerceptionTypeGuid[GuidBufferLength] = { 0 };

static bool g_staticInitialize = []()
{
    (void)StringFromGUID2(MFPoseTrackingPreview::MFPerceptionFormat_PoseTracking, PoseTrackingFormat, _countof(PoseTrackingFormat));
    (void)StringFromGUID2(MFMediaType_Perception, PerceptionTypeGuid, _countof(PerceptionTypeGuid));
    return true;
}();

// Because we are doing a reinterpret cast between different definitions, let's check that they match
static_assert(sizeof(MFPoseTrackingPreview::TrackedPose) == sizeof(WindowsPreview::Media::Capture::Frames::TrackedPose), "Error: Mismatched size of TrackedPose");
static_assert(offsetof(MFPoseTrackingPreview::TrackedPose, PositionStatus) == offsetof(WindowsPreview::Media::Capture::Frames::TrackedPose, PositionStatus), "Error: Mismatched offset of PositionStatus in TrackedPose");
static_assert(offsetof(MFPoseTrackingPreview::TrackedPose, Position) == offsetof(WindowsPreview::Media::Capture::Frames::TrackedPose, Position), "Error: Mismatched offset of Position in TrackedPose");
static_assert(offsetof(MFPoseTrackingPreview::TrackedPose, OrientationStatus) == offsetof(WindowsPreview::Media::Capture::Frames::TrackedPose, OrientationStatus), "Error: Mismatched offset of OrientationStatus in TrackedPose");
static_assert(offsetof(MFPoseTrackingPreview::TrackedPose, Orientation) == offsetof(WindowsPreview::Media::Capture::Frames::TrackedPose, Orientation), "Error: Mismatched offset of v in TrackedPose");

PoseTrackingEntity::PoseTrackingEntity(_In_ Windows::Storage::Streams::IBuffer^ frameBuffer, _In_ unsigned int entityIndex, _In_ const MFPoseTrackingPreview::PoseTrackingEntityData* entityHeader)
    : _frameBuffer(frameBuffer),
    _entityIndex(entityIndex),
    _entityId(entityHeader->EntityId),
    _poseSetId(entityHeader->PoseSet),
    _posesCount(entityHeader->PosesCount),
    _dataSize(entityHeader->DataSizeInBytes),
    _isTracked(entityHeader->IsTracked != 0)
{
}

void PoseTrackingEntity::GetPoses(_Out_ Platform::WriteOnlyArray<TrackedPose>^ poses)
{
    if (poses == nullptr)
    {
        throw ref new NullReferenceException(StringReference(L"poses"));
    }
    if (poses->Length < _posesCount)
    {
        throw ref new InvalidArgumentException(StringReference(L"poses array is too small"));
    }

    BufferHelper helper(_frameBuffer);
    auto frameHeader = helper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingFrameHeader>(0);
    auto entityHeader = helper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingEntityData>(frameHeader->EntityOffsets[_entityIndex]);
    helper.CheckBufferSize<MFPoseTrackingPreview::PoseTrackingEntityData, MFPoseTrackingPreview::TrackedPose>(entityHeader, _posesCount - 1);
    CopyMemory(poses->Data, entityHeader->Poses, sizeof(TrackedPose) * _posesCount);
}

void PoseTrackingEntity::GetPoses(_In_ const Platform::Array<unsigned int>^ indices, _Out_ Platform::WriteOnlyArray<TrackedPose>^ poses)
{
    if (indices == nullptr)
    {
        throw ref new InvalidArgumentException(StringReference(L"indices"));
    }
    if (poses == nullptr)
    {
        throw ref new NullReferenceException(StringReference(L"poses"));
    }
    if (poses->Length < indices->Length)
    {
        throw ref new InvalidArgumentException(StringReference(L"poses array is to small"));
    }

    BufferHelper helper(_frameBuffer);
    auto frameHeader = helper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingFrameHeader>(0);
    auto entityHeader = helper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingEntityData>(frameHeader->EntityOffsets[_entityIndex]);
    helper.CheckBufferSize<MFPoseTrackingPreview::PoseTrackingEntityData, MFPoseTrackingPreview::TrackedPose>(entityHeader, _posesCount - 1);

    // The compatibility of the two types has been verified with the static_assert above.
    auto sourcePoses = reinterpret_cast<const TrackedPose*>(entityHeader->Poses);

    for (unsigned int i = 0; i < indices->Length; i++)
    {
        auto index = indices[i];
        if (index >= _posesCount)
        {
            std::wstring message(L"Invalid Pose Index: ");
            message += std::to_wstring(index);
            throw ref new InvalidArgumentException(ref new String(message.c_str()));
        }
        else
        {
            poses[i] = sourcePoses[index];
        }
    }
}

BufferView^ PoseTrackingEntity::GetCustomData()
{
    BufferHelper bufferHelper(_frameBuffer);
    auto frameHeader = bufferHelper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingFrameHeader>(0);
    auto entityHeader = bufferHelper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingEntityData>(frameHeader->EntityOffsets[_entityIndex]);
    auto entityCommonDataSize = sizeof_composedStructure<MFPoseTrackingPreview::PoseTrackingEntityData, MFPoseTrackingPreview::TrackedPose>(entityHeader->PosesCount);

    auto bufferOffset = frameHeader->EntityOffsets[_entityIndex] + entityCommonDataSize;
    auto bufferLength = entityHeader->DataSizeInBytes - entityCommonDataSize;
    return ref new BufferView(_frameBuffer, bufferOffset, bufferLength);
}

PoseTrackingFrame::PoseTrackingFrame(Windows::Media::Capture::Frames::MediaFrameReference^ mediaFrameReference,
                    _In_ REFIID customDataType,
                    _In_ Windows::Storage::Streams::IBuffer^ frameBuffer,
                    _In_ Windows::Foundation::Collections::IVectorView<PoseTrackingEntity^>^ entities)
    : _mediaFrameReference(mediaFrameReference), _customDataType(customDataType), _frameBuffer(frameBuffer), _entities(entities)
{}

PoseTrackingFrameCreationResult^ PoseTrackingFrame::Create(_In_ Windows::Media::Capture::Frames::MediaFrameReference^ mediaFrameReference)
{
    if (mediaFrameReference == nullptr)
    {
        throw ref new InvalidArgumentException(StringReference(L"mediaFrameReference"));
    }

    // UnknownMediaType may be returned as the MajorType.
    if ((0 != _wcsicmp(mediaFrameReference->Format->MajorType->Data(), UnknownMediaType))
        && (0 != _wcsicmp(mediaFrameReference->Format->MajorType->Data(), PerceptionTypeGuid))
        && (0 != _wcsicmp(mediaFrameReference->Format->MajorType->Data(), PerceptionMediaType))
        )
    {
        return ref new PoseTrackingFrameCreationResult(nullptr, PoseTrackingFrameCreationStatus::NotAPerceptionMediaType);
    }


    if (0 != _wcsicmp(mediaFrameReference->Format->Subtype->Data(), PoseTrackingFormat))
    {
        return ref new PoseTrackingFrameCreationResult(nullptr, PoseTrackingFrameCreationStatus::NotAPoseTrackingFrame);
    }

    auto bmf = mediaFrameReference->BufferMediaFrame;
    if (bmf == nullptr)
    {
        return ref new PoseTrackingFrameCreationResult(nullptr, PoseTrackingFrameCreationStatus::FrameBufferReleased);
    }
    auto buffer = bmf->Buffer;
    if (buffer == nullptr)
    {
        return ref new PoseTrackingFrameCreationResult(nullptr, PoseTrackingFrameCreationStatus::FrameBufferReleased);
    }

    BufferHelper bufferHelper(buffer);
    auto frameHeader = bufferHelper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingFrameHeader>(0);
    auto entities = ref new Vector<PoseTrackingEntity^>(frameHeader->EntitiesCount);

    for (unsigned int index = 0; index < frameHeader->EntitiesCount; index++)
    {
        auto entityHeader = bufferHelper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingEntityData>(frameHeader->EntityOffsets[index]);
        entities->SetAt(index, ref new PoseTrackingEntity(buffer, index, entityHeader));
    }

    return ref new PoseTrackingFrameCreationResult(ref new PoseTrackingFrame(mediaFrameReference, frameHeader->CustomDataTypeGuid, buffer, entities->GetView()), PoseTrackingFrameCreationStatus::Success);
}

BufferView^ PoseTrackingFrame::GetCustomData()
{
    BufferHelper bufferHelper(_frameBuffer);
    auto frameHeader = bufferHelper.GetPtrAt<MFPoseTrackingPreview::PoseTrackingFrameHeader>(0);

    auto bufferOffset = sizeof_composedStructure<MFPoseTrackingPreview::PoseTrackingFrameHeader, uint32_t>(frameHeader->EntitiesCount);
    auto bufferLength = frameHeader->EntityOffsets[0] - bufferOffset;
    return ref new BufferView(_frameBuffer, bufferOffset, bufferLength);
}
