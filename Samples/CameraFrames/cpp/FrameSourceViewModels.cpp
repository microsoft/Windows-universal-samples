//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "FrameSourceViewModels.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;

FrameSourceGroupModel::FrameSourceGroupModel(MediaFrameSourceGroup^ sourceGroup)
{
    SourceGroup = sourceGroup;
    Id = sourceGroup->Id;
    DisplayName = sourceGroup->DisplayName;

    // Populate SourceInfos based on this groups properties.
    Vector<FrameSourceInfoModel^>^ vector = ref new Vector<FrameSourceInfoModel^>();
    for each (auto const& el in sourceGroup->SourceInfos)
    {
        vector->Append(ref new FrameSourceInfoModel(el));
    }
    SourceInfos = vector->GetView();
}

FrameSourceInfoModel::FrameSourceInfoModel(MediaFrameSourceInfo^ sourceInfo)
{
    SourceInfo = sourceInfo;
    SourceGroup = sourceInfo->SourceGroup;
    Id = sourceInfo->Id;
    DisplayName = SourceInfo->SourceKind.ToString() + " " + SourceInfo->MediaStreamType.ToString();
}

FrameFormatModel::FrameFormatModel(MediaFrameFormat^ format)
{
    Format = format;

    String^ width = Format->VideoFormat ? Format->VideoFormat->Width.ToString() : "?";
    String^ height = Format->VideoFormat ? Format->VideoFormat->Height.ToString() : "?";

    String^ framerate = Format->FrameRate ? round(Format->FrameRate->Numerator / Format->FrameRate->Denominator).ToString() : "?";

    DisplayName = Format->MajorType + " | " + Format->Subtype + " | " + width + " x " + height + " | " + framerate + "fps";
}

SourceGroupCollection::SourceGroupCollection(Windows::UI::Core::CoreDispatcher ^ uiDispatcher)
{
    m_dispatcher = uiDispatcher;
    m_sourceCollection = ref new Vector<FrameSourceGroupModel^>();

    m_watcher = DeviceInformation::CreateWatcher(MediaFrameSourceGroup::GetDeviceSelector());

    // On source group added, add new source group to source groups collection.
    m_watcherDeviceAddedToken = m_watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(
        [this](DeviceWatcher^ sender, DeviceInformation^ args)
    {
        AddDeviceAsync(args->Id);
    });

    // On source group removed, remove source group from source groups collection.
    m_watcherDeviceRemovedToken = m_watcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ args)
    {
        RemoveDeviceAsync(args->Id);
    });

    // On source group updated, re-add source group to source groups collection.
    m_watcherDeviceUpdatedToken = m_watcher->Updated += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ args)
    {
        String^ id = args->Id;
        RemoveDeviceAsync(id).then([this, id]()
        {
            return AddDeviceAsync(id);
        });
    });

    m_watcher->Start();
}

SourceGroupCollection::~SourceGroupCollection()
{
    m_watcher->Added -= m_watcherDeviceAddedToken;
    m_watcher->Removed -= m_watcherDeviceRemovedToken;
    m_watcher->Updated -= m_watcherDeviceUpdatedToken;

    m_watcher->Stop();
}

concurrency::task<void> SourceGroupCollection::AddDeviceAsync(Platform::String ^ id)
{
    // If source group is present in collection already, there is no need to add it.
    for (UINT32 i = 0; i < m_sourceCollection->Size; i++)
    {
        if (m_sourceCollection->GetAt(i)->Id == id)
        {
            return task_from_result();
        }
    }

    // Look up source group and add it to the collection.
    return create_task(MediaFrameSourceGroup::FromIdAsync(id)).then([this](MediaFrameSourceGroup^ group)
    {
        return create_task(m_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
            ref new Windows::UI::Core::DispatchedHandler([this, group]()
        {
            if (group != nullptr)
            {
                m_sourceCollection->Append(ref new FrameSourceGroupModel(group));
            }
        })));
    });
}

concurrency::task<void> SourceGroupCollection::RemoveDeviceAsync(Platform::String^ id)
{
    return create_task(m_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
        ref new Windows::UI::Core::DispatchedHandler([this, id]()
    {
        // Remove source group from the collection.
        for (UINT32 i = 0; i < m_sourceCollection->Size; i++)
        {
            if (m_sourceCollection->GetAt(i)->Id == id)
            {
                m_sourceCollection->RemoveAt(i);
            }
        }
    })));
}
