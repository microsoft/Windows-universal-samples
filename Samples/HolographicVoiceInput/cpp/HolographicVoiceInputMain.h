#pragma once
//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//********************************************************* 



//
// Comment out this preprocessor definition to disable all of the
// sample content.
//
// To remove the content after disabling it:
//     * Remove the unused code from your app's Main class.
//     * Delete the Content folder provided with this template.
//

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"

#include "Content\SpinningCubeRenderer.h"
#include "Content\SpatialInputHandler.h"

#include "Audio/OmnidirectionalSound.h"

// Updates, renders, and presents holographic content using Direct3D.
namespace HolographicVoiceInput
{
    class HolographicVoiceInputMain : public DX::IDeviceNotify
    {
    public:
        HolographicVoiceInputMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~HolographicVoiceInputMain();

        // Sets the holographic space. This is our closest analogue to setting a new window
        // for the app.
        void SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);

        // Starts the holographic frame and updates the content.
        Windows::Graphics::Holographic::HolographicFrame^ Update();

        // Renders holograms, including world-locked content.
        bool Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame);

        // Handle saving and loading of app state owned by AppMain.
        void SaveAppState();
        void LoadAppState();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        // Process continuous speech recognition results.
        void OnResultGenerated(
            Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender,
            Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^args
        );

        // Recognize when conditions might impact speech recognition quality.
        void OnSpeechQualityDegraded(
            Windows::Media::SpeechRecognition::SpeechRecognizer^ recognizer,
            Windows::Media::SpeechRecognition::SpeechRecognitionQualityDegradingEventArgs^ args
        );

        // Initializes the speech command list.
        void InitializeSpeechCommandList();

        // Initializes a speech recognizer.
        bool InitializeSpeechRecognizer();

        // Provides a voice prompt, before starting the scenario.
        void BeginVoiceUIPrompt();
        void PlayRecognitionBeginSound();
        void PlayRecognitionSound();

        // Creates a speech command recognizer, and starts listening.
        Concurrency::task<bool> StartRecognizeSpeechCommands();

        // Resets the speech recognizer, if one exists.
        Concurrency::task<void> StopCurrentRecognizerIfExists();

        // Asynchronously creates resources for new holographic cameras.
        void OnCameraAdded(
            Windows::Graphics::Holographic::HolographicSpace^ sender,
            Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs^ args);

        // Synchronously releases resources for holographic cameras that are no longer
        // attached to the system.
        void OnCameraRemoved(
            Windows::Graphics::Holographic::HolographicSpace^ sender,
            Windows::Graphics::Holographic::HolographicSpaceCameraRemovedEventArgs^ args);

        // Used to notify the app when the positional tracking state changes.
        void OnLocatabilityChanged(
            Windows::Perception::Spatial::SpatialLocator^ sender,
            Platform::Object^ args);

        // Clears event registration state. Used when changing to a new HolographicSpace
        // and when tearing down AppMain.
        void UnregisterHolographicEventHandlers();

        // Renders a colorful holographic cube that's 20 centimeters wide. This sample content
        // is used to demonstrate world-locked rendering.
        std::unique_ptr<SpinningCubeRenderer>                           m_spinningCubeRenderer;

        // Listens for the Pressed spatial input event.
        std::shared_ptr<SpatialInputHandler>                            m_spatialInputHandler;

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                            m_deviceResources;

        // Render loop timer.
        DX::StepTimer                                                   m_timer;

        // Represents the holographic space around the user.
        Windows::Graphics::Holographic::HolographicSpace^               m_holographicSpace;

        // SpatialLocator that is attached to the primary camera.
        Windows::Perception::Spatial::SpatialLocator^                   m_locator;

        // A reference frame attached to the holographic camera.
        Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ m_referenceFrame;

        // Event registration tokens.
        Windows::Foundation::EventRegistrationToken                     m_cameraAddedToken;
        Windows::Foundation::EventRegistrationToken                     m_cameraRemovedToken;
        Windows::Foundation::EventRegistrationToken                     m_locatabilityChangedToken;

        Windows::Foundation::EventRegistrationToken                     m_speechRecognizerResultEventToken;
        Windows::Foundation::EventRegistrationToken                     m_speechRecognitionQualityDegradedToken;

        bool                                                            m_listening;

        // Speech recognizer.
        Windows::Media::SpeechRecognition::SpeechRecognizer^            m_speechRecognizer;

        // Maps commands to color data.
        // We will create a Vector of the key values in this map for use as speech commands.
        Platform::Collections::Map<Platform::String^, Windows::Foundation::Numerics::float4>^ m_speechCommandData;

        // The most recent speech command received.
        Platform::String^                                               m_lastCommand;

        // Handles playback of speech synthesis audio.
        OmnidirectionalSound                                            m_speechSynthesisSound;
        OmnidirectionalSound                                            m_recognitionSound;
        OmnidirectionalSound                                            m_startRecognitionSound;

        bool                                                            m_waitingForSpeechPrompt = false;
        bool                                                            m_waitingForSpeechCue = false;
        float                                                           m_secondsUntilSoundIsComplete = 0.f;
    };
}
