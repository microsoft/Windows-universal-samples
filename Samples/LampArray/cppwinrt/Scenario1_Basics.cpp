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
#include "Scenario1_Basics.h"
#include "Scenario1_Basics.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Lights;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Basics::Scenario1_Basics()
    {
        InitializeComponent();
    }

    void Scenario1_Basics::OnNavigatedTo(NavigationEventArgs const&)
    {
        UpdateLampArrayList();

        // Start watching for newly attached LampArrays.
        m_deviceWatcher = DeviceInformation::CreateWatcher(LampArray::GetDeviceSelector());

        m_deviceAddedRevoker = m_deviceWatcher.Added(winrt::auto_revoke, { get_weak(), &Scenario1_Basics::Watcher_Added});
        m_deviceRemovedRevoker = m_deviceWatcher.Removed(winrt::auto_revoke, { get_weak(), &Scenario1_Basics::Watcher_Removed });

        m_deviceWatcher.Start();
    }

    void Scenario1_Basics::OnNavigatedFrom(NavigationEventArgs const&)
    {
        m_deviceWatcher.Stop();
    }

    winrt::fire_and_forget Scenario1_Basics::Watcher_Added(DeviceWatcher const&, DeviceInformation const& deviceInformation)
    {
        auto lifetime = get_strong();

        auto info = std::make_shared<LampArrayInfo>(
            deviceInformation.Id(),
            deviceInformation.Name(),
            LampArray::FromIdAsync(deviceInformation.Id()).get());

        // DeviceWatcher events are invoked on a background thread.
        // We need to switch to the foreground thread to update our app UI
        // and access member variables without race conditions.

        co_await winrt::resume_foreground(Dispatcher());

        if (info->lampArray == nullptr)
        {
            // A LampArray was found, but Windows couldn't initialize it.
            // This suggests a device error.
            rootPage.NotifyUser(L"Problem with LampArray " + info->displayName + L".", NotifyType::ErrorMessage);
            co_return;
        }

        // Set up the AvailabilityChanged event callback for being notified whether this process can control
        // RGB lighting for the newly attached LampArray.
        info->availabilityChangedRevoker = info->lampArray.AvailabilityChanged(winrt::auto_revoke, { this, &Scenario1_Basics::LampArray_AvailabilityChanged });

        // Remember this new LampArray and add it to the list.
        m_attachedLampArrays.push_back(std::move(info));
        UpdateLampArrayList();
    }

    winrt::fire_and_forget Scenario1_Basics::Watcher_Removed(DeviceWatcher const&, DeviceInformationUpdate deviceInformationUpdate)
    {
        auto lifetime = get_strong();

        // DeviceWatcher events are invoked on a background thread.
        // We need to switch to the foreground thread to update our app UI
        // and access member variables without race conditions.
        co_await winrt::resume_foreground(Dispatcher());

        m_attachedLampArrays.erase(
            std::remove_if(m_attachedLampArrays.begin(), m_attachedLampArrays.end(), [id = deviceInformationUpdate.Id()](auto&& info) { return info->id == id; }),
            m_attachedLampArrays.end());

        UpdateLampArrayList();
    }

    // The AvailabilityChanged event will fire when this calling process gains or loses control of RGB lighting
    // for the specified LampArray.
    winrt::fire_and_forget Scenario1_Basics::LampArray_AvailabilityChanged(const LampArray& sender, const IInspectable&)
    {
        co_await winrt::resume_foreground(Dispatcher());

        UpdateLampArrayList();
    }

    void Scenario1_Basics::UpdateLampArrayList()
    {
        std::wstring message = std::wstring(L"Attached LampArrays: ") + std::to_wstring(m_attachedLampArrays.size()) + L"\n";

        for (auto&& info : m_attachedLampArrays)
        {
            message += std::wstring(L"\t") + std::wstring(info->displayName) + L" ("
                + std::to_wstring(info->lampArray.LampArrayKind()) + L", " + std::to_wstring(info->lampArray.LampCount()) + L" lamps, "
                + std::wstring(info->lampArray.IsAvailable() ? L"Available" : L"Unavailable") + L")\n";
        }

        LampArraysSummary().Text(message);
    }

    void Scenario1_Basics::Apply_Clicked(IInspectable const&, RoutedEventArgs const&)
    {
        for (auto&& info : m_attachedLampArrays)
        {
            ApplySettingsToLampArray(info->lampArray);
        }
    }

    void Scenario1_Basics::ApplySettingsToLampArray(LampArray const& lampArray)
    {
        // Apply the light pattern.
        if (OffButton().IsChecked().Value())
        {
            // Set all Lamps on the LampArray to black, turning them off.
            lampArray.SetColor(Colors::Black());
        }
        else if (SetColorButton().IsChecked().Value())
        {
            // Set all Lamps on the LampArray to green.
            lampArray.SetColor(Colors::Green());
        }
        else if (GradientButton().IsChecked().Value())
        {
            SetGradientPatternToLampArray(lampArray);
        }
        else if (WasdButton().IsChecked().Value())
        {
            SetWasdPatternToLampArray(lampArray);
        }

        // Apply the brightness. Convert the percentage value in the slider to the range [0, 1] expected by BrightnessLevel.
        lampArray.BrightnessLevel(BrightnessSlider().Value() / BrightnessSlider().Maximum());
    }

    void Scenario1_Basics::SetGradientPatternToLampArray(LampArray const& lampArray)
    {
        // Create a static gradient on the LampArray from black to red, blending from left to right.

        // For this effect, we want to set the colors of all Lamps. Create an array containing all the Lamp indices
        // and an array of Colors. Both arrays must be of the same length. We will use these for SetColorsForIndices later.
        std::vector<int> indices(lampArray.LampCount());
        std::iota(indices.begin(), indices.end(), 0);

        std::vector<Color> colors(lampArray.LampCount());

        // A LampArray provides information about its size using a three-dimensional logical bounding box,
        // as well as the location of each of its Lamps within that bounding box.
        // The origin point of the bounding box is the leftmost (X), farthest (Y), uppermost (Z) corner relative to the user.
        // LampArray bounding boxes and Lamp positions are measured in meters.

        // Because our gradient will blend from left to right, we will set the Lamp colors according to each Lamp's X position.
        // We will make two passes over the LampArray. First, calculate the minimum and maximum X position values of all Lamps.
        // This is to "trim" the margins of the bounding box where no Lamps are present.
        double maxX = 0;
        double minX = lampArray.BoundingBox().x;

        for (int i : indices)
        {
            LampInfo lampInfo = lampArray.GetLampInfo(i);

            if (lampInfo.Position().x > maxX)
            {
                maxX = lampInfo.Position().x;
            }

            if (lampInfo.Position().x < minX)
            {
                minX = lampInfo.Position().x;
            }
        }

        // In the second pass, we will calculate the gradient colors for each Lamp.
        for (int i : indices)
        {
            LampInfo lampInfo = lampArray.GetLampInfo(i);

            // Calculate the X position for this Lamp relative to the rightmost Lamp. We will scale the Lamp's color by this relative value below.
            // (We also want to avoid dividing by zero here in case all Lamps share the same X position, including the single-Lamp case.)
            double xProgress = 1.0;
            if (maxX != minX)
            {
                xProgress = (lampInfo.Position().x - minX) / (maxX - minX);
            }

            // Apply that value to the expected color for the lamp.
            // Maximize the Alpha value to give the color full opacity.
            colors[i].A = std::numeric_limits<uint8_t>::max();

            // Set the color's R value based on the Lamp's X position.
            // As the X position of the Lamp increases (i.e., is further to the right),
            // the Red value of the Lamp's color will also increase.
            // This means that the leftmost Lamp(s) will be black (turned off), and the rightmost Lamp(s) will be the brightest red.
            colors[i].R = (uint8_t)(xProgress * std::numeric_limits<uint8_t>::max());
        }

        lampArray.SetColorsForIndices(colors, indices);
    }

    void Scenario1_Basics::SetWasdPatternToLampArray(LampArray const& lampArray)
    {
        // Set a background color of blue.
        lampArray.SetColor(Colors::Blue());

        // Highlight the WASD keys in white, if the LampArray supports addressing its Lamps using a virtual key to Lamp mapping.
        // This is typically found on keyboard LampArrays. Other LampArrays will not usually support virtual key based lighting.
        if (lampArray.SupportsVirtualKeys())
        {
            auto white = Colors::White();
            std::array<Color, 4> colors{ white, white, white, white };
            std::array<VirtualKey, 4> virtualKeys{ VirtualKey::W, VirtualKey::A, VirtualKey::S, VirtualKey::D };

            lampArray.SetColorsForKeys(colors, virtualKeys);
        }
    }
}
