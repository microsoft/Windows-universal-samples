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

#include "Scenario2_Effects.g.h"

#include <numeric>

namespace winrt::SDKTemplate::implementation
{
    // State for the custom wave effect.
    struct CustomEffectState
    {
        CustomEffectState(winrt::Windows::Devices::Lights::LampArray const& lampArray) :
            lampArray(lampArray),
            lampNormalizedX(lampArray.LampCount()),
            lampColors(lampNormalizedX.size(), winrt::Windows::UI::Colors::Black())
        { }

        winrt::Windows::Devices::Lights::LampArray lampArray{ nullptr };
        uint32_t frameNumber{ 0 };

        // A vector of each lamp's X position within the bounding box, in the range [0, 1],
        // where 0 is the leftmost edge and 1 is the rightmost edge.
        std::vector<float> lampNormalizedX;

        // A vector of colors to be applied to each Lamp in the effect callback function.
        // The vector is ordered by Lamp index. The ordering does not reflect positions of any Lamp.
        std::vector<winrt::Windows::UI::Color> lampColors;

        winrt::Windows::UI::Color color{ winrt::Windows::UI::Colors::HotPink() };
        bool isColorOn{ true };
    };

    // State for the generated bitmap effect.
    struct GeneratedBitmapState
    {
        uint32_t bitmapWidth;
        uint32_t bitmapHeight;
        float squareLength;
        float squareY;
        uint32_t frameNumber{ 0 };

        winrt::Microsoft::Graphics::Canvas::CanvasDevice canvasDevice{ nullptr };
        winrt::Microsoft::Graphics::Canvas::CanvasRenderTarget offscreenCanvas{ nullptr };
        winrt::Microsoft::Graphics::Canvas::CanvasDrawingSession offscreenDrawingSession{ nullptr };

        winrt::Windows::Graphics::Imaging::SoftwareBitmap squareBitmap{ nullptr };
        winrt::Windows::Storage::Streams::Buffer squareBuffer{ nullptr };
    };

    struct Scenario2_Effects : Scenario2_EffectsT<Scenario2_Effects>
    {
    public:
        Scenario2_Effects();

        void OnNavigatedTo(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        winrt::fire_and_forget StaticBitmapButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void FadeButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void CustomEffectButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void GeneratedBitmapButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void CycleButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void PauseButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void PlayButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void StopButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        winrt::Windows::Devices::Enumeration::DeviceWatcher m_deviceWatcher{ nullptr };

        // Currently attached LampArrays
        std::vector<std::unique_ptr<LampArrayInfo>> m_attachedLampArrays;

        winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Lights::Effects::LampArrayEffectPlaylist> m_emptyList =
            winrt::single_threaded_vector< winrt::Windows::Devices::Lights::Effects::LampArrayEffectPlaylist>().GetView();

        // Playlists that can be paused, stopped, and started.
        winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Lights::Effects::LampArrayEffectPlaylist> m_readyPlaylists = m_emptyList;

        std::default_random_engine m_random{ std::random_device()() };

        winrt::Windows::Devices::Enumeration::DeviceWatcher::Added_revoker m_deviceAddedRevoker{};
        winrt::Windows::Devices::Enumeration::DeviceWatcher::Removed_revoker m_deviceRemovedRevoker{};

        winrt::fire_and_forget Watcher_Added(
            winrt::Windows::Devices::Enumeration::DeviceWatcher const&,
            winrt::Windows::Devices::Enumeration::DeviceInformation const& deviceInformation);

        winrt::fire_and_forget Watcher_Removed(
            winrt::Windows::Devices::Enumeration::DeviceWatcher const&,
            winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInformationUpdate);

        winrt::fire_and_forget LampArray_AvailabilityChanged(
            winrt::Windows::Devices::Lights::LampArray const& sender,
            winrt::Windows::Foundation::IInspectable const&);

        void UpdateLampArrayList();
        void CleanupPreviousEffect();

        static void StaticBitmapEffect_BitmapRequested(
            winrt::Windows::Graphics::Imaging::SoftwareBitmap const& bitmap,
            winrt::Windows::Devices::Lights::Effects::LampArrayBitmapRequestedEventArgs const& args);

        static void CustomEffect_UpdateRequested(
            CustomEffectState* state,
            winrt::Windows::Devices::Lights::Effects::LampArrayUpdateRequestedEventArgs const& args);

        static void GeneratedBitmapEffect_UpdateRequested(
            GeneratedBitmapState* state,
            winrt::Windows::Devices::Lights::Effects::LampArrayBitmapRequestedEventArgs const& args);

        void StartStopAllEffects(bool shouldStart);

        static std::vector<int> GetLampArrayIndices(winrt::Windows::Devices::Lights::LampArray const& lampArray);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Effects : Scenario2_EffectsT<Scenario2_Effects, implementation::Scenario2_Effects>
    {
    };
}
