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

#include "Scenario2_Effects.h"
#include "Scenario2_Effects.g.cpp"

using namespace winrt;
using namespace winrt::Microsoft::Graphics::Canvas;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Lights;
using namespace winrt::Windows::Devices::Lights::Effects;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace
{
    inline constexpr TimeSpan EffectUpdateInterval = std::chrono::milliseconds(33);
    inline constexpr TimeSpan ColorRampDuration = std::chrono::milliseconds(500);
    inline constexpr int CustomEffectFrameCount = 30;
    inline constexpr int GeneratedBitmapEffectFrameCount = 60;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Effects::Scenario2_Effects()
    {
        InitializeComponent();
    }

    void Scenario2_Effects::OnNavigatedTo(NavigationEventArgs const&)
    {
        UpdateLampArrayList();

        // Start watching for newly attached LampArrays.
        m_deviceWatcher = DeviceInformation::CreateWatcher(LampArray::GetDeviceSelector());

        m_deviceAddedRevoker = m_deviceWatcher.Added(winrt::auto_revoke, { get_weak(), &Scenario2_Effects::Watcher_Added });
        m_deviceRemovedRevoker = m_deviceWatcher.Removed(winrt::auto_revoke, { get_weak(), &Scenario2_Effects::Watcher_Removed });

        m_deviceWatcher.Start();
    }

    void Scenario2_Effects::OnNavigatedFrom(NavigationEventArgs const&)
    {
        m_deviceWatcher.Stop();

        // When changing scenarios, stop our effects to prevent them from interfering with the new scenario.
        CleanupPreviousEffect();
    }

    winrt::fire_and_forget Scenario2_Effects::Watcher_Added(DeviceWatcher const&, DeviceInformation const& deviceInformation)
    {
        auto lifetime = get_strong();

        auto info = std::make_unique<LampArrayInfo>(
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

        // Initial condition for the new LampArray is all lights off.
        info->lampArray.SetColor(Colors::Black());

        // Set up the AvailabilityChanged event callback for being notified whether this process can control
        // RGB lighting for the newly attached LampArray.
        info->availabilityChangedRevoker = info->lampArray.AvailabilityChanged(winrt::auto_revoke, { this, &Scenario2_Effects::LampArray_AvailabilityChanged });

        m_attachedLampArrays.push_back(std::move(info));

        UpdateLampArrayList();
    }

    winrt::fire_and_forget Scenario2_Effects::Watcher_Removed(DeviceWatcher const&, DeviceInformationUpdate deviceInformationUpdate)
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
    winrt::fire_and_forget Scenario2_Effects::LampArray_AvailabilityChanged(const LampArray& sender, const IInspectable&)
    {
        co_await winrt::resume_foreground(Dispatcher());

        UpdateLampArrayList();
    }

    void Scenario2_Effects::UpdateLampArrayList()
    {
        std::wstring message = std::wstring(L"Attached LampArrays: ") + std::to_wstring(m_attachedLampArrays.size()) + L"\n";

        for (auto&& info : m_attachedLampArrays)
        {
            message += L"\t" + info->displayName + L" ("
                + std::to_wstring(info->lampArray.LampArrayKind()) + L", " + std::to_wstring(info->lampArray.LampCount()) + L" lamps, "
                + std::wstring(info->lampArray.IsAvailable() ? L"Available" : L"Unavailable") + L")\n";
        }

        LampArraysSummary().Text(message);
    }

    // This method will be called when a new effect is selected. It will stop any running
    // LampArrayEffectPlaylists in progress and reset LampArray state in preparation for the new effect.
    void Scenario2_Effects::CleanupPreviousEffect()
    {
        // Clear the bitmap image in the app UI, as it is no longer current.
        ImageBitmap().Source(nullptr);

        // Passing all the playlists to a single call to StopAll will stop them
        // simultaneously across all attached LampArrays.
        LampArrayEffectPlaylist::StopAll(m_readyPlaylists);
        m_readyPlaylists = m_emptyList;
    }

#pragma region Static bitmap effect
    winrt::fire_and_forget Scenario2_Effects::StaticBitmapButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        CleanupPreviousEffect();

        // This effect will display a specified bitmap image on all connected LampArrays.
        // The image will be stretched and scaled to fit the Lamp dimensions of each LampArray.

        // First, open the desired bitmap file and convert it to a SoftwareBitmap
        auto bitmapUri = Uri(L"ms-appx:///Assets/grapes.jpg");

        StorageFile file = co_await StorageFile::GetFileFromApplicationUriAsync(bitmapUri);
        IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::Read);
        BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(stream);
        SoftwareBitmap bitmap = co_await decoder.GetSoftwareBitmapAsync();
        stream.Close();

        // Show our bitmap image in the app UI
        ImageBitmap().Source(BitmapImage(bitmapUri));

        // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
        std::vector<LampArrayEffectPlaylist> playlists;

        // Create the effect playlist for each LampArray and add it to the collection.
        for (auto&& info : m_attachedLampArrays)
        {
            LampArrayBitmapEffect bitmapEffect(info->lampArray, GetLampArrayIndices(info->lampArray));

            // We want to run this effect until is stopped. For the purposes of this sample,
            // set the effect Duration to run as long as possible.
            bitmapEffect.Duration(TimeSpan::max());

            // The UpdateInterval specifies how frequently we want to receive our callback function.
            // Since this effect will display a static image, we only need to update once.
            // Set this interval to be the same as the effect duration.
            bitmapEffect.UpdateInterval(TimeSpan::max());

            // Set the event handler for our effect. Use a helper function to
            // to specify a bitmap to display on the device.
            bitmapEffect.BitmapRequested([bitmap](auto&&, auto&& args)
            {
                StaticBitmapEffect_BitmapRequested(bitmap, args);
            });

            // Create a playlist consisting of our static bitmap effect.
            LampArrayEffectPlaylist playlist;
            playlist.Append(bitmapEffect);

            // Add it to the list of playlists to start simultaneously.
            playlists.push_back(playlist);
        }

        // Start the newly created effect playlists simultaneously.
        auto playlistsView = winrt::single_threaded_vector(std::move(playlists)).GetView();
        LampArrayEffectPlaylist::StartAll(playlistsView);

        // Remember the playlists so we can stop them later.
        m_readyPlaylists = playlistsView;
    }

    // Helper function for BitmapRequested event handler that
    // specifies the bitmap to display on the device.
    // The frequency of the effect callback is customizable.
    // The frequency of the event is customizable.
    // As configured above when setting up the static bitmap effect, this event will
    // be raised only once per effect iteration, since we set the update interval
    // equal to the effect duration.
    void Scenario2_Effects::StaticBitmapEffect_BitmapRequested(
        SoftwareBitmap const& bitmap,
        LampArrayBitmapRequestedEventArgs const& args)
    {
        // In a BitmapRequested handler, we can change the bitmap here to produce animations.
        // For the static bitmap effect, show the same image each time.
        args.UpdateBitmap(bitmap);
    }
#pragma endregion

#pragma region Fade effect
    void Scenario2_Effects::FadeButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CleanupPreviousEffect();

        // This effect will set each Lamp to a random color and blink all Lamps simultaneously.
        // LampArrayBlinkEffect only supports a single color at a time. To create the desired effect with
        // different colors for each Lamp, we will create a separate LampArrayBlinkEffect for each Lamp,
        // specifying that we only want the effect to apply to a single Lamp index.
        // We will then add each Lamp effect to the playlist for its corresponding LampArray and configure the
        // playlist to run its effects simultaneously instead of sequentially.

        // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
        std::vector<LampArrayEffectPlaylist> playlists;

        auto uniform_byte = std::uniform_int_distribution(0, 255);

        // Create the effect playlist for each LampArray and add it to the collection.
        for (auto&& info : m_attachedLampArrays)
        {
            LampArrayEffectPlaylist playlist;

            // Configure the playlist to run the effects simultaneously.
            // This will result in all Lamps for this LampArray blinking in unison,
            // even though their colors will be different.
            playlist.EffectStartMode(LampArrayEffectStartMode::Simultaneous);

            for (int i = 0; i < info->lampArray.LampCount(); i++)
            {
                // Generate a color for each Lamp by choosing a random Red/Green/Blue combination.
                auto color = ColorHelper::FromArgb(
                    std::numeric_limits<uint8_t>::max(), // Maximum alpha makes it opaque
                    (uint8_t)(uniform_byte(m_random)),
                    (uint8_t)(uniform_byte(m_random)),
                    (uint8_t)(uniform_byte(m_random)));

                // Create the effect and apply it to a single Lamp.
                LampArrayBlinkEffect blinkEffect(info->lampArray, { i });

                blinkEffect.Color(color);

                // We can adjust the timing of the LampArrayBlinkEffect using these properties.
                // Specify how long it should take for the full color to fade in from off/Black to peak intensity.
                blinkEffect.AttackDuration(std::chrono::milliseconds(300));

                // Specify how long to display the color at peak intensity.
                blinkEffect.SustainDuration(std::chrono::milliseconds(500));

                // Specify how long it should take for the full color to fade out from peak intensity to off/Black.
                blinkEffect.DecayDuration(std::chrono::milliseconds(800));

                // Specify how long the Lamps should stay dark between blink repetitions.
                blinkEffect.RepetitionDelay(std::chrono::milliseconds(100));

                // Repeat the blink sequence indefinitely until the effect playlist is stopped.
                blinkEffect.RepetitionMode(LampArrayRepetitionMode::Forever);

                playlist.Append(blinkEffect);

                // Add it to the list of playlists to start simultaneously.
                playlists.push_back(playlist);
            }
        }

        // Start the newly created effect playlists simultaneously.
        auto playlistsView = winrt::single_threaded_vector(std::move(playlists)).GetView();
        LampArrayEffectPlaylist::StartAll(playlistsView);

        // Remember the playlists so we can stop them later.
        m_readyPlaylists = playlistsView;
    }
#pragma endregion

#pragma region Custom effect
    // Windows.Devices.Lights.Effects contains several built-in effect types. The LampArrayCustomEffect is provided for
    // greater customization of lighting beyond these simple effects. It enables the SetColor controls of LampArray to
    // be integrated and scheduled with other effects and effect playlists.
    // This sample will demonstrate using LampArrayCustomEffect to display a simple lighting animation.
    void Scenario2_Effects::CustomEffectButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CleanupPreviousEffect();

        // This effect will use LampArrayCustomEffect to create a wave-like animation.
        // Lamps will turn on from left to right across the LampArray, then turn off in a similar manner.

        // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
        std::vector<LampArrayEffectPlaylist> playlists;

        // Create the effect playlist for each LampArray and add it to the collection.
        for (auto&& info : m_attachedLampArrays)
        {
            LampArray const& lampArray = info->lampArray;
            std::vector<int> indices = GetLampArrayIndices(lampArray);
            float boundingBoxWidth = lampArray.BoundingBox().x;
            // Avoid division by zero when calculating normalized positions.
            if (boundingBoxWidth == 0.0f)
            {
                boundingBoxWidth = 1.0f;
            }
            auto state = std::make_unique<CustomEffectState>(lampArray);

            // Calculate the normalized position of each lamp.
            for (int i = 0; i < state->lampNormalizedX.size(); i++)
            {
                auto lampInfo = lampArray.GetLampInfo(i);
                state->lampNormalizedX[i] = lampInfo.Position().x / boundingBoxWidth;
            }

            LampArrayCustomEffect customEffect = LampArrayCustomEffect(lampArray, indices);

            // We want to run this effect until is stopped. For the purposes of this sample,
            // set the effect Duration to run as long as possible.
            customEffect.Duration(TimeSpan::max());

            // The UpdateInterval specifies how frequently we want to receive our callback function.
            // For our animation, set this interval to 33 milliseconds, which equates to approximately 30 frames per second.
            customEffect.UpdateInterval(EffectUpdateInterval);

            // This event handler will be called on every frame to update the animation.
            customEffect.UpdateRequested([state = std::move(state)](auto&&, auto&& args)
            {
                CustomEffect_UpdateRequested(state.get(), args);
            });

            // Create a playlist consisting of our wave effect.
            LampArrayEffectPlaylist playlist;
            playlist.Append(customEffect);

            // Add it to the list of playlists to start simultaneously.
            playlists.push_back(playlist);
        }

        // Start the newly created effect playlists.
        auto playlistsView = winrt::single_threaded_vector(std::move(playlists)).GetView();
        LampArrayEffectPlaylist::StartAll(playlistsView);

        // Remember the playlists so we can stop them later.
        m_readyPlaylists = playlistsView;
    }

    // Helper for the LampArrayCustomEffect.UpdateRequested event, called periodically at the requested UpdateInterval.
    // This enables more complex lighting patterns, such as animations. It also supports integration and scheduling with
    // other effects and effect playlists.
    // As configured above, this callback will be invoked every 33 milliseconds, or about 30 frames per second.
    void Scenario2_Effects::CustomEffect_UpdateRequested(
        CustomEffectState* state,
        winrt::Windows::Devices::Lights::Effects::LampArrayUpdateRequestedEventArgs const& args)
    {
        // Update the Lamp colors for the current frame. Start by calculating how far into the current iteration we are.
        // We will use this to mark how far across the LampArray bounding box the effect has reached.
        double effectProgress = ((state->frameNumber + 1) / static_cast<double>(CustomEffectFrameCount));

        // If Lamps were being turned off in the previous iteration, turn them on now, and vice versa.
        Color color = state->isColorOn ? state->color : Colors::Black();

        for (unsigned i = 0; i < state->lampNormalizedX.size(); i++)
        {
            // All Lamps that are to the left of our marker (normalized to the LampArray bounding box width)
            // should have their states updated.
            if (state->lampNormalizedX[i] <= effectProgress)
            {
                state->lampColors[i] = color;
            }
        }

        // Apply the Lamp states to the LampArray.
        args.SetColorsForIndices(state->lampColors, GetLampArrayIndices(state->lampArray));

        // Increment our frame counter, with wraparound if we've reached the frame limit.
        // Also update whether the lamps should be turned on or off for the next iteration.
        if (++state->frameNumber == CustomEffectFrameCount)
        {
            state->frameNumber = 0;
            state->isColorOn = !state->isColorOn;
        }
    }
#pragma endregion

#pragma region Generated bitmap effect
    // This effect will use Win2D to create a simple animation. On each frame, we will
    // generate a bitmap image and apply it to the entire LampArray.
    // The effect illustrates dynamically creating a bitmap image and using it in a lighting effect.
    // LampArrayBitmapEffect could be used for in-game lighting and/or integration with 2D graphics libraries.
    void Scenario2_Effects::GeneratedBitmapButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CleanupPreviousEffect();

        // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
        std::vector<LampArrayEffectPlaylist> playlists;

        // Create the effect playlist for each LampArray and add it to the collection.
        for (auto&& info : m_attachedLampArrays)
        {
            auto state = std::make_unique<GeneratedBitmapState>();

            auto generatedBitmapEffect = LampArrayBitmapEffect{ info->lampArray, GetLampArrayIndices(info->lampArray) };

            // We want to run this effect until is stopped. For the purposes of this sample,
            // set the effect Duration to run as long as possible.
            generatedBitmapEffect.Duration(TimeSpan::max());

            // The UpdateInterval specifies how frequently we want to receive our callback function.
            // For our animation, set this interval to 33 milliseconds, which equates to approximately 30 frames per second.
            generatedBitmapEffect.UpdateInterval(EffectUpdateInterval);

            Size suggestedSize = generatedBitmapEffect.SuggestedBitmapSize();
            state->bitmapWidth = static_cast<uint32_t>(suggestedSize.Width);
            state->bitmapHeight = static_cast<uint32_t>(suggestedSize.Height);

            // The size of the square is half the height or half the width, whichever is smaller.
            state->squareLength = std::min(suggestedSize.Height, suggestedSize.Width) / 2.0f;

            // Vertically center the square.
            state->squareY = (suggestedSize.Height - state->squareLength) / 2.0f;

            // Setup our Win2D helper objects.
            state->canvasDevice = CanvasDevice::GetSharedDevice();

            state->offscreenCanvas = CanvasRenderTarget(state->canvasDevice, suggestedSize.Width, suggestedSize.Height, 96.0f);

            state->offscreenDrawingSession = state->offscreenCanvas.CreateDrawingSession();

            // The generated bitmap encompasses the entire LampArray
            state->squareBitmap = SoftwareBitmap(BitmapPixelFormat::Bgra8, state->bitmapWidth, state->bitmapHeight, BitmapAlphaMode::Premultiplied);

            state->squareBuffer = Buffer(state->bitmapWidth * state->bitmapHeight * 4);

            // This event handler will be called on every frame to update the bitmap to display on the device.
            generatedBitmapEffect.BitmapRequested([state = std::move(state)](auto&&, auto&& args)
            {
                GeneratedBitmapEffect_UpdateRequested(state.get(), args);
            });

            // Create a playlist consisting of our bitmap effect.
            LampArrayEffectPlaylist playlist;
            playlist.Append(generatedBitmapEffect);

            // Add it to the list of playlists to start simultaneously.
            playlists.push_back(playlist);
        }

        // Start the newly created effect playlists simultaneously.
        auto playlistsView = winrt::single_threaded_vector(std::move(playlists)).GetView();
        LampArrayEffectPlaylist::StartAll(playlistsView);

        // Remember the playlists so we can stop them later.
        m_readyPlaylists = playlistsView;
    }

    // Callback function that allows us to specify a bitmap to display on the device.
    // The frequency of the effect callback is customizable.
    // As configured above when setting up the generated bitmap effect, this will be invoked
    // every 33 milliseconds, or approximately 30 frames per second.
    // To demonstrate using a LampArrayBitmapEffect to perform lighting animations,
    // this callback will create an animation of a red rectangle moving left to right
    // across a blue background.
    void Scenario2_Effects::GeneratedBitmapEffect_UpdateRequested(
        GeneratedBitmapState* state,
        LampArrayBitmapRequestedEventArgs const& args)
    {
        // Draw a red rectangle on a blue background.
        state->offscreenDrawingSession.Clear(Colors::Blue());

        // The rectangle starts out just beyond the left edge of the keyboard,
        // then moves to the right until it just leaves the keyboard,
        // and then repeats.
        float squareX = static_cast<float>(state->frameNumber * (state->bitmapWidth + state->squareLength) / GeneratedBitmapEffectFrameCount) - state->squareLength;
        state->offscreenDrawingSession.FillRectangle(
            squareX,
            state->squareY,
            state->squareLength,
            state->squareLength,
            Colors::Red());

        state->offscreenDrawingSession.Flush();

        // Apply the generated bitmap to the LampArray.
        state->offscreenCanvas.GetPixelBytes(state->squareBuffer);
        state->squareBitmap.CopyFromBuffer(state->squareBuffer);

        args.UpdateBitmap(state->squareBitmap);

        // Advance to the next frame for the next iteration.
        if (++state->frameNumber == GeneratedBitmapEffectFrameCount)
        {
            state->frameNumber = 0;
        }
    }
#pragma endregion

#pragma region Color cycle effect
    void Scenario2_Effects::CycleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CleanupPreviousEffect();

        // This effect will use LampArrayColorRampEffect to perform a rainbow cycle pattern on all Lamps.
        // We will do this for each LampArray by creating a LampArrayEffectPlaylist containing four LampArrayColorRampEffects.
        // Each effect will blend from the previously set color to a new color over a 500ms interval.

        // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
        std::vector<LampArrayEffectPlaylist> playlists;

        // Create the effect playlist for each LampArray and add it to the collection.
        for (auto&& info : m_attachedLampArrays)
        {
            // Create a playlist to hold our effects.
            LampArrayEffectPlaylist playlist;
            playlist.RepetitionMode(LampArrayRepetitionMode::Forever);
            playlist.EffectStartMode(LampArrayEffectStartMode::Sequential);

            auto indices = GetLampArrayIndices(info->lampArray);

            LampArrayColorRampEffect redRampEffect{ info->lampArray, indices };
            redRampEffect.Color(Colors::Red());
            redRampEffect.RampDuration(ColorRampDuration);
            redRampEffect.CompletionBehavior(LampArrayEffectCompletionBehavior::KeepState);
            playlist.Append(redRampEffect);

            LampArrayColorRampEffect yellowRampEffect{ info->lampArray, indices };
            yellowRampEffect.Color(Colors::Yellow());
            yellowRampEffect.RampDuration(ColorRampDuration);
            yellowRampEffect.CompletionBehavior(LampArrayEffectCompletionBehavior::KeepState);
            playlist.Append(yellowRampEffect);

            LampArrayColorRampEffect greenRampEffect{ info->lampArray, indices };
            greenRampEffect.Color(Colors::Green());
            greenRampEffect.RampDuration(ColorRampDuration);
            greenRampEffect.CompletionBehavior(LampArrayEffectCompletionBehavior::KeepState);
            playlist.Append(greenRampEffect);

            LampArrayColorRampEffect blueRampEffect{ info->lampArray, indices };
            blueRampEffect.Color(Colors::Blue());
            blueRampEffect.RampDuration(ColorRampDuration);
            blueRampEffect.CompletionBehavior(LampArrayEffectCompletionBehavior::KeepState);
            playlist.Append(blueRampEffect);

            // Add this multi-effect playlist to the list of playlists to start simultaneously.
            playlists.push_back(playlist);
        }

        // Start the newly created effect playlists simultaneously.
        auto playlistsView = winrt::single_threaded_vector(std::move(playlists)).GetView();
        LampArrayEffectPlaylist::StartAll(playlistsView);

        // Remember the playlists so we can stop them later.
        m_readyPlaylists = playlistsView;
    }
#pragma endregion

    void Scenario2_Effects::PauseButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        LampArrayEffectPlaylist::PauseAll(m_readyPlaylists);
    }

    void Scenario2_Effects::PlayButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        LampArrayEffectPlaylist::StartAll(m_readyPlaylists);
    }

    void Scenario2_Effects::StopButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        LampArrayEffectPlaylist::StopAll(m_readyPlaylists);
    }

    // Helper function that returns all indices of a LampArray in order.
    // Used when we want to apply an effect to all Lamps on a LampArray.
    std::vector<int> Scenario2_Effects::GetLampArrayIndices(winrt::Windows::Devices::Lights::LampArray const& lampArray)
    {
        std::vector<int> indices(lampArray.LampCount());
        std::iota(indices.begin(), indices.end(), 0);
        return indices;
    }
}
