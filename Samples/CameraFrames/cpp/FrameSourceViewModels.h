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

#pragma once

namespace SDKTemplate
{
    ref class FrameSourceGroupModel;
    ref class FrameSourceInfoModel;
    ref class FrameFormatModel;

    /// <summary>
    /// View model for MediaFrameSourceGroup used in XAML ContentControl.
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class FrameSourceGroupModel sealed
    {
    public:
        FrameSourceGroupModel(Windows::Media::Capture::Frames::MediaFrameSourceGroup^ sourceGroup);

    public: // Public properties.
        property Platform::String^ Id;
        property Platform::String^ DisplayName;
        property Windows::Media::Capture::Frames::MediaFrameSourceGroup^ SourceGroup;
        property Windows::Foundation::Collections::IVectorView<FrameSourceInfoModel^>^ SourceInfos;
    };

    /// <summary>
    /// View model for MediaFrameSourceInfo used in XAML ContentControl.
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class FrameSourceInfoModel sealed
    {
    public:
        FrameSourceInfoModel(Windows::Media::Capture::Frames::MediaFrameSourceInfo^ sourceInfo);

        property Platform::String^ DisplayName;
        property Platform::String^ Id;
        property Windows::Media::Capture::Frames::MediaFrameSourceGroup^ SourceGroup;
        property Windows::Media::Capture::Frames::MediaFrameSourceInfo^ SourceInfo;
    };

    /// <summary>
    /// View model for MediaFrameFormat used in XAML ContentControl.
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class FrameFormatModel sealed
    {
    public:
        FrameFormatModel(Windows::Media::Capture::Frames::MediaFrameFormat^ format);

        /// <summary>
        /// Compares the Format contained by this view model to the given format for equivalency.
        /// </summary>
        /// <param name="otherFormat">The MediaFrameFormat to compare to the MediaFrameFormat in this view model.</param>
        /// <returns></returns>
        bool HasSameFormat(Windows::Media::Capture::Frames::MediaFrameFormat^ otherFormat)
        {
            if (otherFormat == nullptr)
            {
                return (Format == nullptr);
            }

            return Format->MajorType == otherFormat->MajorType &&
                Format->Subtype == otherFormat->Subtype &&
                Format->FrameRate->Numerator == otherFormat->FrameRate->Numerator &&
                Format->FrameRate->Denominator == otherFormat->FrameRate->Denominator &&
                ((Format->VideoFormat == nullptr && otherFormat->VideoFormat == nullptr) ||
                 (Format->VideoFormat != nullptr && otherFormat->VideoFormat != nullptr &&
                  Format->VideoFormat->Width == otherFormat->VideoFormat->Width &&
                  Format->VideoFormat->Height == otherFormat->VideoFormat->Height));
        }

    public: // Public properties.
        property Platform::String^ DisplayName;
        property Windows::Media::Capture::Frames::MediaFrameFormat^ Format;
    };

    /// <summary>
    /// View model for a collection of MediaFrameSourceGroup used in XAML ItemsControl.
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    public ref class SourceGroupCollection sealed
    {
    public:
        SourceGroupCollection(Windows::UI::Core::CoreDispatcher^ uiDispatcher);
        virtual ~SourceGroupCollection();

    public: // public properties
        property Windows::Foundation::Collections::IObservableVector<FrameSourceGroupModel^>^ FrameSourceGroups
        {
            Windows::Foundation::Collections::IObservableVector<FrameSourceGroupModel^>^ get()
            {
                return m_sourceCollection;
            };
        };

    private: // private methods
        /// <summary>
        /// Adds a SourceGroup with given Id to the collection.
        /// </summary>
        concurrency::task<void> AddDeviceAsync(Platform::String^ id);

        /// <summary>
        /// Removes a SourceGroup with given id from the collection if it exists
        /// </summary>
        concurrency::task<void> RemoveDeviceAsync(Platform::String^ id);

    private: // private data
        Windows::Foundation::Collections::IObservableVector<FrameSourceGroupModel^>^ m_sourceCollection;

        Windows::UI::Core::CoreDispatcher^ m_dispatcher;

        Windows::Devices::Enumeration::DeviceWatcher^ m_watcher;
        Windows::Foundation::EventRegistrationToken m_watcherDeviceAddedToken;
        Windows::Foundation::EventRegistrationToken m_watcherDeviceRemovedToken;
        Windows::Foundation::EventRegistrationToken m_watcherDeviceUpdatedToken;
    };
} // SDKTemplate