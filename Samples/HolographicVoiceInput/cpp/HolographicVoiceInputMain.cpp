//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//********************************************************* 

#include "pch.h"
#include "HolographicVoiceInputMain.h"
#include "Common\DirectXHelper.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include <Collection.h>

#include "Audio/OmnidirectionalSound.h"

using namespace HolographicVoiceInput;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;

// Loads and initializes application assets when the application is loaded.
HolographicVoiceInputMain::HolographicVoiceInputMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    // Initialize the map of speech commands to actions - in this case, color values.
    InitializeSpeechCommandList();

}


void HolographicVoiceInputMain::InitializeSpeechCommandList()
{
    m_lastCommand = nullptr;
    m_listening = false;
    m_speechCommandData = ref new Platform::Collections::Map<String^, float4>();

    m_speechCommandData->Insert(L"white", float4(1.0f, 1.0f, 1.0f, 1.f));
    m_speechCommandData->Insert(L"grey", float4(0.5f, 0.5f, 0.5f, 1.f));
    m_speechCommandData->Insert(L"green", float4(0.0f, 1.0f, 0.0f, 1.f));
    m_speechCommandData->Insert(L"black", float4(0.1f, 0.1f, 0.1f, 1.f));
    m_speechCommandData->Insert(L"red", float4(1.0f, 0.0f, 0.0f, 1.f));
    m_speechCommandData->Insert(L"yellow", float4(1.0f, 1.0f, 0.0f, 1.f));
    m_speechCommandData->Insert(L"aquamarine", float4(0.0f, 1.0f, 1.0f, 1.f));
    m_speechCommandData->Insert(L"blue", float4(0.0f, 0.0f, 1.0f, 1.f));
    m_speechCommandData->Insert(L"purple", float4(1.0f, 0.0f, 1.0f, 1.f));

    // You can use non-dictionary words as speech commands.
    m_speechCommandData->Insert(L"SpeechRecognizer", float4(0.5f, 0.1f, 1.f, 1.f));
}

void HolographicVoiceInputMain::BeginVoiceUIPrompt()
{
    // RecognizeWithUIAsync provides speech recognition begin and end prompts, but it does not provide
    // synthesized speech prompts. Instead, you should provide your own speech prompts when requesting
    // phrase input.
    // Here is an example of how to do that with a speech synthesizer. You could also use a pre-recorded 
    // voice clip, a visual UI, or other indicator of what to say.
    auto speechSynthesizer = ref new Windows::Media::SpeechSynthesis::SpeechSynthesizer();

    StringReference voicePrompt;

    // A command list is used to continuously look for one-word commands.
    // You need some way for the user to know what commands they can say. In this example, we provide
    // verbal instructions; you could also use graphical UI, etc.
    voicePrompt = L"Say the name of a color, at any time, to change the color of the cube.";

    // Kick off speech synthesis.
    create_task(speechSynthesizer->SynthesizeTextToStreamAsync(voicePrompt), task_continuation_context::use_current())
        .then([this, speechSynthesizer](task<Windows::Media::SpeechSynthesis::SpeechSynthesisStream^> synthesisStreamTask)
    {
        try
        {
            // The speech synthesis is sent as a byte stream.
            Windows::Media::SpeechSynthesis::SpeechSynthesisStream^ stream = synthesisStreamTask.get();

            // We can initialize an XAudio2 voice using that byte stream.
            // Here, we use it to play an HRTF audio effect.
            auto hr = m_speechSynthesisSound.Initialize(stream, 0);
            if (SUCCEEDED(hr))
            {
                m_speechSynthesisSound.SetEnvironment(HrtfEnvironment::Small);
                m_speechSynthesisSound.Start();

                // Amount of time to pause after the audio prompt is complete, before listening 
                // for speech input.
                static const float bufferTime = 0.15f;

                // Wait until the prompt is done before listening.
                m_secondsUntilSoundIsComplete = m_speechSynthesisSound.GetDuration() + bufferTime;
                m_waitingForSpeechPrompt = true;
            }
        }
        catch (Exception^ exception)
        {
            PrintWstringToDebugConsole(
                std::wstring(L"Exception while trying to synthesize speech: ") +
                exception->Message->Data() +
                L"\n"
            );

            // Handle exceptions here.
        }
    });
}

void HolographicVoiceInputMain::PlayRecognitionBeginSound()
{
    // The user needs a cue to begin speaking. We will play this sound effect just before starting 
    // the recognizer.
    auto hr = m_startRecognitionSound.GetInitializationStatus();
    if (SUCCEEDED(hr))
    {
        m_startRecognitionSound.SetEnvironment(HrtfEnvironment::Small);
        m_startRecognitionSound.Start();

        // Wait until the audible cue is done before starting to listen.
        m_secondsUntilSoundIsComplete = m_startRecognitionSound.GetDuration();
        m_waitingForSpeechCue = true;
    }
}

void HolographicVoiceInputMain::PlayRecognitionSound()
{
    // The user should be given a cue when recognition is complete. 
    auto hr = m_recognitionSound.GetInitializationStatus();
    if (SUCCEEDED(hr))
    {
        // re-initialize the sound so it can be replayed.
        m_recognitionSound.Initialize(L"Audio//BasicResultsEarcon.wav", 0);
        m_recognitionSound.SetEnvironment(HrtfEnvironment::Small);
        
        m_recognitionSound.Start();
    }
}

Concurrency::task<void> HolographicVoiceInputMain::StopCurrentRecognizerIfExists()
{
    if (m_speechRecognizer != nullptr)
    {
        return create_task(m_speechRecognizer->StopRecognitionAsync()).then([this]()
        {
            m_speechRecognizer->RecognitionQualityDegrading -= m_speechRecognitionQualityDegradedToken;

            if (m_speechRecognizer->ContinuousRecognitionSession != nullptr)
            {
                m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= m_speechRecognizerResultEventToken;
            }
        });
    }
    else
    {
        return create_task([this]() { });
    }
}

bool HolographicVoiceInputMain::InitializeSpeechRecognizer()
{
    m_speechRecognizer = ref new SpeechRecognizer();

    if (!m_speechRecognizer)
    {
        return false;
    }

    m_speechRecognitionQualityDegradedToken = m_speechRecognizer->RecognitionQualityDegrading +=
        ref new TypedEventHandler<SpeechRecognizer^, SpeechRecognitionQualityDegradingEventArgs^>(
            std::bind(&HolographicVoiceInputMain::OnSpeechQualityDegraded, this, _1, _2)
            );

    m_speechRecognizerResultEventToken = m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
        ref new TypedEventHandler<SpeechContinuousRecognitionSession^, SpeechContinuousRecognitionResultGeneratedEventArgs^>(
            std::bind(&HolographicVoiceInputMain::OnResultGenerated, this, _1, _2)
            );

    return true;
}

task<bool> HolographicVoiceInputMain::StartRecognizeSpeechCommands()
{
    return StopCurrentRecognizerIfExists().then([this]()
    {
        if (!InitializeSpeechRecognizer())
        {
            return task_from_result<bool>(false);
        }

        // Here, we compile the list of voice commands by reading them from the map.
        Platform::Collections::Vector<String^>^ speechCommandList = ref new Platform::Collections::Vector<String^>();
        for each (auto pair in m_speechCommandData)
        {
            // The speech command string is what we are looking for here. Later, we can use the
            // recognition result for this string to look up a color value.
            auto command = pair->Key;

            // Add it to the list.
            speechCommandList->Append(command);
        }

        SpeechRecognitionListConstraint^ spConstraint = ref new SpeechRecognitionListConstraint(speechCommandList);
        m_speechRecognizer->Constraints->Clear();
        m_speechRecognizer->Constraints->Append(spConstraint);
        return create_task(m_speechRecognizer->CompileConstraintsAsync()).then([this](task<SpeechRecognitionCompilationResult^> previousTask)
        {
            try
            {
                SpeechRecognitionCompilationResult^ compilationResult = previousTask.get();

                if (compilationResult->Status == SpeechRecognitionResultStatus::Success)
                {
                    // If compilation succeeds, we can start listening for results.
                    return create_task(m_speechRecognizer->ContinuousRecognitionSession->StartAsync()).then([this](task<void> startAsyncTask) {
                        
                        try
                        {
                            // StartAsync may throw an exception if your app doesn't have Microphone permissions. 
                            // Make sure they're caught and handled appropriately (otherwise the app may silently not work as expected)
                            startAsyncTask.get();
                            return true;
                        }
                        catch (Exception^ exception)
                        {
                            PrintWstringToDebugConsole(
                                std::wstring(L"Exception while trying to start speech Recognition: ") +
                                exception->Message->Data() +
                                L"\n"
                            );

                            return false;
                        }
                    });
                }
                else
                {
                    OutputDebugStringW(L"Could not initialize constraint-based speech engine!\n");

                    // Handle errors here.
                    return create_task([this] {return false; });
                }
            }
            catch (Exception^ exception)
            {
                // Note that if you get an "Access is denied" exception, you might need to enable the microphone 
                // privacy setting on the device and/or add the microphone capability to your app manifest.

                PrintWstringToDebugConsole(
                    std::wstring(L"Exception while trying to initialize speech command list:") +
                    exception->Message->Data() +
                    L"\n"
                );

                // Handle exceptions here.
                return create_task([this] {return false; });
            }
        });
    });
}


void HolographicVoiceInputMain::SetHolographicSpace(HolographicSpace^ holographicSpace)
{
    UnregisterHolographicEventHandlers();

    m_holographicSpace = holographicSpace;

    //
    // TODO: Add code here to initialize your holographic content.
    //

    // Initialize the sample hologram.
    m_spinningCubeRenderer = std::make_unique<SpinningCubeRenderer>(m_deviceResources);

    m_spatialInputHandler = std::make_unique<SpatialInputHandler>();

    // Use the default SpatialLocator to track the motion of the device.
    m_locator = SpatialLocator::GetDefault();

    // Be able to respond to changes in the positional tracking state.
    m_locatabilityChangedToken =
        m_locator->LocatabilityChanged +=
            ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
                std::bind(&HolographicVoiceInputMain::OnLocatabilityChanged, this, _1, _2)
                );

    // Respond to camera added events by creating any resources that are specific
    // to that camera, such as the back buffer render target view.
    // When we add an event handler for CameraAdded, the API layer will avoid putting
    // the new camera in new HolographicFrames until we complete the deferral we created
    // for that handler, or return from the handler without creating a deferral. This
    // allows the app to take more than one frame to finish creating resources and
    // loading assets for the new holographic camera.
    // This function should be registered before the app creates any HolographicFrames.
    m_cameraAddedToken =
        m_holographicSpace->CameraAdded +=
            ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraAddedEventArgs^>(
                std::bind(&HolographicVoiceInputMain::OnCameraAdded, this, _1, _2)
                );

    // Respond to camera removed events by releasing resources that were created for that
    // camera.
    // When the app receives a CameraRemoved event, it releases all references to the back
    // buffer right away. This includes render target views, Direct2D target bitmaps, and so on.
    // The app must also ensure that the back buffer is not attached as a render target, as
    // shown in DeviceResources::ReleaseResourcesForBackBuffer.
    m_cameraRemovedToken =
        m_holographicSpace->CameraRemoved +=
            ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraRemovedEventArgs^>(
                std::bind(&HolographicVoiceInputMain::OnCameraRemoved, this, _1, _2)
                );

    // The simplest way to render world-locked holograms is to create a stationary reference frame
    // when the app is launched. This is roughly analogous to creating a "world" coordinate system
    // with the origin placed at the device's position as the app is launched.
    m_referenceFrame = m_locator->CreateStationaryFrameOfReferenceAtCurrentLocation();

    // Notes on spatial tracking APIs:
    // * Stationary reference frames are designed to provide a best-fit position relative to the
    //   overall space. Individual positions within that reference frame are allowed to drift slightly
    //   as the device learns more about the environment.
    // * When precise placement of individual holograms is required, a SpatialAnchor should be used to
    //   anchor the individual hologram to a position in the real world - for example, a point the user
    //   indicates to be of special interest. Anchor positions do not drift, but can be corrected; the
    //   anchor will use the corrected position starting in the next frame after the correction has
    //   occurred.

    // Preload audio assets for audio cues.
    m_startRecognitionSound.Initialize(L"Audio//BasicListeningEarcon.wav", 0);
    m_recognitionSound.Initialize(L"Audio//BasicResultsEarcon.wav", 0);

    // Begin the code sample scenario.
    BeginVoiceUIPrompt();
}

void HolographicVoiceInputMain::UnregisterHolographicEventHandlers()
{
    if (m_holographicSpace != nullptr)
    {
        // Clear previous event registrations.

        if (m_cameraAddedToken.Value != 0)
        {
            m_holographicSpace->CameraAdded -= m_cameraAddedToken;
            m_cameraAddedToken.Value = 0;
        }

        if (m_cameraRemovedToken.Value != 0)
        {
            m_holographicSpace->CameraRemoved -= m_cameraRemovedToken;
            m_cameraRemovedToken.Value = 0;
        }
    }

    if (m_locator != nullptr)
    {
        m_locator->LocatabilityChanged -= m_locatabilityChangedToken;
    }
}

HolographicVoiceInputMain::~HolographicVoiceInputMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);

    UnregisterHolographicEventHandlers();
}

// Updates the application state once per frame.
HolographicFrame^ HolographicVoiceInputMain::Update()
{
    // Before doing the timer update, there is some work to do per-frame
    // to maintain holographic rendering. First, we will get information
    // about the current frame.

    // The HolographicFrame has information that the app needs in order
    // to update and render the current frame. The app begins each new
    // frame by calling CreateNextFrame.
    HolographicFrame^ holographicFrame = m_holographicSpace->CreateNextFrame();

    // Get a prediction of where holographic cameras will be when this frame
    // is presented.
    HolographicFramePrediction^ prediction = holographicFrame->CurrentPrediction;

    // Back buffers can change from frame to frame. Validate each buffer, and recreate
    // resource views and depth buffers as needed.
    m_deviceResources->EnsureCameraResources(holographicFrame, prediction);

    // Next, we get a coordinate system from the attached frame of reference that is
    // associated with the current frame. Later, this coordinate system is used for
    // for creating the stereo view matrices when rendering the sample content.
    SpatialCoordinateSystem^ currentCoordinateSystem = m_referenceFrame->CoordinateSystem;

    // Check for new input state since the last frame.
    SpatialInteractionSourceState^ pointerState = m_spatialInputHandler->CheckForInput();
    if (pointerState != nullptr)
    {
        // When a Pressed gesture is detected, the sample hologram will be repositioned
        // two meters in front of the user.
        m_spinningCubeRenderer->PositionHologram(
            pointerState->TryGetPointerPose(currentCoordinateSystem)
            );
    }

    // Check for new speech input since the last frame.
    if (m_lastCommand != nullptr)
    {
        auto command = m_lastCommand;
        m_lastCommand = nullptr;

        // Check to see if the spoken word or phrase, matches up with any of the speech
        // commands in our speech command map.
        for each (auto& iter in m_speechCommandData)
        {
            std::wstring lastCommandString = command->Data();
            std::wstring listCommandString = iter->Key->Data();

            if (lastCommandString.find(listCommandString) != std::wstring::npos)
            {
                // If so, we can set the cube to the color that was spoken.
                m_spinningCubeRenderer->SetColor(iter->Value);
                break;
            }
        }
    }


    m_timer.Tick([&] ()
    {
        //
        // TODO: Update scene objects.
        //
        // Put time-based updates here. By default this code will run once per frame,
        // but if you change the StepTimer to use a fixed time step this code will
        // run as many times as needed to get to the current step.
        //

        m_spinningCubeRenderer->Update(m_timer);

        // Wait to listen for speech input until the audible UI prompts are complete.
        if ((m_waitingForSpeechPrompt == true) &&
            ((m_secondsUntilSoundIsComplete -= static_cast<float>(m_timer.GetElapsedSeconds())) <= 0.f))
        {
            m_waitingForSpeechPrompt = false;
            PlayRecognitionBeginSound();
        }
        else if ((m_waitingForSpeechCue == true) &&
            ((m_secondsUntilSoundIsComplete -= static_cast<float>(m_timer.GetElapsedSeconds())) <= 0.f))
        {
            m_waitingForSpeechCue = false;
            m_secondsUntilSoundIsComplete = 0.f;
            StartRecognizeSpeechCommands();
        }
        
    });

    // We complete the frame update by using information about our content positioning
    // to set the focus point.

    for (auto cameraPose : prediction->CameraPoses)
    {
        // The HolographicCameraRenderingParameters class provides access to set
        // the image stabilization parameters.
        HolographicCameraRenderingParameters^ renderingParameters = holographicFrame->GetRenderingParameters(cameraPose);

        // SetFocusPoint informs the system about a specific point in your scene to
        // prioritize for image stabilization. The focus point is set independently
        // for each holographic camera.
        // You should set the focus point near the content that the user is looking at.
        // In this example, we put the focus point at the center of the sample hologram,
        // since that is the only hologram available for the user to focus on.
        // You can also set the relative velocity and facing of that content; the sample
        // hologram is at a fixed point so we only need to indicate its position.
        renderingParameters->SetFocusPoint(
            currentCoordinateSystem,
            m_spinningCubeRenderer->GetPosition()
            );
    }

    // The holographic frame will be used to get up-to-date view and projection matrices and
    // to present the swap chain.
    return holographicFrame;
}

// Renders the current frame to each holographic camera, according to the
// current application and spatial positioning state. Returns true if the
// frame was rendered to at least one camera.
bool HolographicVoiceInputMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return false;
    }

    //
    // TODO: Add code for pre-pass rendering here.
    //
    // Take care of any tasks that are not specific to an individual holographic
    // camera. This includes anything that doesn't need the final view or projection
    // matrix, such as lighting maps.
    //

    // Lock the set of holographic camera resources, then draw to each camera
    // in this frame.
    return m_deviceResources->UseHolographicCameraResources<bool>(
        [this, holographicFrame](std::map<UINT32, std::unique_ptr<DX::CameraResources>>& cameraResourceMap)
    {
        // Up-to-date frame predictions enhance the effectiveness of image stablization and
        // allow more accurate positioning of holograms.
        holographicFrame->UpdateCurrentPrediction();
        HolographicFramePrediction^ prediction = holographicFrame->CurrentPrediction;

        bool atLeastOneCameraRendered = false;
        for (auto cameraPose : prediction->CameraPoses)
        {
            // This represents the device-based resources for a HolographicCamera.
            DX::CameraResources* pCameraResources = cameraResourceMap[cameraPose->HolographicCamera->Id].get();

            // Get the device context.
            const auto context = m_deviceResources->GetD3DDeviceContext();
            const auto depthStencilView = pCameraResources->GetDepthStencilView();

            // Set render targets to the current holographic camera.
            ID3D11RenderTargetView *const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
            context->OMSetRenderTargets(1, targets, depthStencilView);

            // Clear the back buffer and depth stencil view.
            context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
            context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            //
            // TODO: Replace the sample content with your own content.
            //
            // Notes regarding holographic content:
            //    * For drawing, remember that you have the potential to fill twice as many pixels
            //      in a stereoscopic render target as compared to a non-stereoscopic render target
            //      of the same resolution. Avoid unnecessary or repeated writes to the same pixel,
            //      and only draw holograms that the user can see.
            //    * To help occlude hologram geometry, you can create a depth map using geometry
            //      data obtained via the surface mapping APIs. You can use this depth map to avoid
            //      rendering holograms that are intended to be hidden behind tables, walls,
            //      monitors, and so on.
            //    * Black pixels will appear transparent to the user wearing the device, but you
            //      should still use alpha blending to draw semitransparent holograms. You should
            //      also clear the screen to Transparent as shown above.
            //


            // The view and projection matrices for each holographic camera will change
            // every frame. This function refreshes the data in the constant buffer for
            // the holographic camera indicated by cameraPose.
            pCameraResources->UpdateViewProjectionBuffer(m_deviceResources, cameraPose, m_referenceFrame->CoordinateSystem);

            // Attach the view/projection constant buffer for this camera to the graphics pipeline.
            bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

            // Only render world-locked content when positional tracking is active.
            if (cameraActive)
            {
                // Draw the sample hologram.
                m_spinningCubeRenderer->Render();
            }
            atLeastOneCameraRendered = true;
        }

        return atLeastOneCameraRendered;
    });
}

void HolographicVoiceInputMain::SaveAppState()
{
    //
    // TODO: Insert code here to save your app state.
    //       This method is called when the app is about to suspend.
    //
    //       For example, store information in the SpatialAnchorStore.
    //
}

void HolographicVoiceInputMain::LoadAppState()
{
    //
    // TODO: Insert code here to load your app state.
    //       This method is called when the app resumes.
    //
    //       For example, load information from the SpatialAnchorStore.
    //
}

// Notifies classes that use Direct3D device resources that the device resources
// need to be released before this method returns.
void HolographicVoiceInputMain::OnDeviceLost()
{
    m_spinningCubeRenderer->ReleaseDeviceDependentResources();
}

// Notifies classes that use Direct3D device resources that the device resources
// may now be recreated.
void HolographicVoiceInputMain::OnDeviceRestored()
{
    m_spinningCubeRenderer->CreateDeviceDependentResources();
}

void HolographicVoiceInputMain::OnLocatabilityChanged(SpatialLocator^ sender, Object^ args)
{
    switch (sender->Locatability)
    {
    case SpatialLocatability::Unavailable:
        // Holograms cannot be rendered.
        {
            String^ message = L"Warning! Positional tracking is " +
                                        sender->Locatability.ToString() + L".\n";
            OutputDebugStringW(message->Data());
        }
        break;

    // In the following three cases, it is still possible to place holograms using a
    // SpatialLocatorAttachedFrameOfReference.
    case SpatialLocatability::PositionalTrackingActivating:
        // The system is preparing to use positional tracking.

    case SpatialLocatability::OrientationOnly:
        // Positional tracking has not been activated.

    case SpatialLocatability::PositionalTrackingInhibited:
        // Positional tracking is temporarily inhibited. User action may be required
        // in order to restore positional tracking.
        break;

    case SpatialLocatability::PositionalTrackingActive:
        // Positional tracking is active. World-locked content can be rendered.
        break;
    }
}

void HolographicVoiceInputMain::OnCameraAdded(
    HolographicSpace^ sender,
    HolographicSpaceCameraAddedEventArgs^ args
    )
{
    Deferral^ deferral = args->GetDeferral();
    HolographicCamera^ holographicCamera = args->Camera;
    create_task([this, deferral, holographicCamera] ()
    {
        //
        // TODO: Allocate resources for the new camera and load any content specific to
        //       that camera. Note that the render target size (in pixels) is a property
        //       of the HolographicCamera object, and can be used to create off-screen
        //       render targets that match the resolution of the HolographicCamera.
        //

        // Create device-based resources for the holographic camera and add it to the list of
        // cameras used for updates and rendering. Notes:
        //   * Since this function may be called at any time, the AddHolographicCamera function
        //     waits until it can get a lock on the set of holographic camera resources before
        //     adding the new camera. At 60 frames per second this wait should not take long.
        //   * A subsequent Update will take the back buffer from the RenderingParameters of this
        //     camera's CameraPose and use it to create the ID3D11RenderTargetView for this camera.
        //     Content can then be rendered for the HolographicCamera.
        m_deviceResources->AddHolographicCamera(holographicCamera);

        // Holographic frame predictions will not include any information about this camera until
        // the deferral is completed.
        deferral->Complete();
    });
}

void HolographicVoiceInputMain::OnCameraRemoved(
    HolographicSpace^ sender,
    HolographicSpaceCameraRemovedEventArgs^ args
    )
{
    create_task([this]()
    {
        //
        // TODO: Asynchronously unload or deactivate content resources (not back buffer 
        //       resources) that are specific only to the camera that was removed.
        //
    });

    // Before letting this callback return, ensure that all references to the back buffer 
    // are released.
    // Since this function may be called at any time, the RemoveHolographicCamera function
    // waits until it can get a lock on the set of holographic camera resources before
    // deallocating resources for this camera. At 60 frames per second this wait should
    // not take long.
    m_deviceResources->RemoveHolographicCamera(args->Camera);
}


// For speech example.
// Change the cube color, if we get a valid result.
void HolographicVoiceInputMain::OnResultGenerated(SpeechContinuousRecognitionSession ^sender, SpeechContinuousRecognitionResultGeneratedEventArgs ^args)
{
    // For our list of commands, medium confidence is good enough. 
    // We also accept results that have high confidence.
    if ((args->Result->Confidence == SpeechRecognitionConfidence::High) ||
        (args->Result->Confidence == SpeechRecognitionConfidence::Medium))
    {
        m_lastCommand = args->Result->Text;

        // When the debugger is attached, we can print information to the debug console.
        PrintWstringToDebugConsole(
            std::wstring(L"Last command was: ") +
            m_lastCommand->Data() +
            L"\n"
        );

        // Play a sound to indicate a command was understood.
        PlayRecognitionSound();
    }
    else
    {
        OutputDebugStringW(L"Recognition confidence not high enough.\n");
    }
}

void HolographicVoiceInputMain::OnSpeechQualityDegraded(Windows::Media::SpeechRecognition::SpeechRecognizer^ recognizer, Windows::Media::SpeechRecognition::SpeechRecognitionQualityDegradingEventArgs^ args)
{
    switch (args->Problem)
    {
    case SpeechRecognitionAudioProblem::TooFast:
        OutputDebugStringW(L"The user spoke too quickly.\n");
        break;

    case SpeechRecognitionAudioProblem::TooSlow:
        OutputDebugStringW(L"The user spoke too slowly.\n");
        break;

    case SpeechRecognitionAudioProblem::TooQuiet:
        OutputDebugStringW(L"The user spoke too softly.\n");
        break;

    case SpeechRecognitionAudioProblem::TooLoud:
        OutputDebugStringW(L"The user spoke too loudly.\n");
        break;

    case SpeechRecognitionAudioProblem::TooNoisy:
        OutputDebugStringW(L"There is too much noise in the signal.\n");
        break;

    case SpeechRecognitionAudioProblem::NoSignal:
        OutputDebugStringW(L"There is no signal.\n");
        break;

    case SpeechRecognitionAudioProblem::None:
    default:
        OutputDebugStringW(L"An error was reported with no information.\n");
        break;
    }
}