#include "pch.h"
#include "SpatialControllerHandler.h"
#include "Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Devices::Haptics;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;

SpatialControllerHandler::SpatialControllerHandler(SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target)
    : SourceHandler(source, eventsHandler, target)
{
    SpatialInteractionController^ controller = source->Controller;
    if (controller != nullptr)
    {
        // Give Thumbstuck priority over Touchpad for tool selection.
        if (controller->HasThumbstick)
        {
            m_spDPadHelper = std::make_shared<ThumbstickDPadHelper>();
        }
        else if (controller->HasTouchpad)
        {
            m_spDPadHelper = std::make_shared<TouchpadDPadHelper>();
        }
        // Find out of the controller support haptics to be able to send vibrations when entering / leaving the board
        TryGetContinousBuzz(controller->SimpleHapticsController);
    }

    m_gestureRecognizer = ref new SpatialGestureRecognizer(SpatialGestureSettings::Tap);
    // Handle Tap Gesture to leave tool selection mode
    m_tappedToken =
        m_gestureRecognizer->Tapped
            += ref new TypedEventHandler<SpatialGestureRecognizer^, SpatialTappedEventArgs^>(
                std::bind(&SpatialControllerHandler::OnTapped, this, _1, _2));
}

SpatialControllerHandler::~SpatialControllerHandler()
{
    m_gestureRecognizer->Tapped -= m_tappedToken;
}

float3 SpatialControllerHandler::GetToolScale() const
{
    if (m_currentTool == nullptr)
    {
        return float3(DefaultControllerScale);
    }
    // Make the tool almost flat (5mm) as we are joinging squares when drawing
    return float3(m_currentTool->GetDiameter(), 0.005f, m_currentTool->GetDiameter());
}

void SpatialControllerHandler::OnInteractionDetected(SpatialInteraction^ interaction, SpatialCoordinateSystem^ coordinateSystem)
{
    switch (m_currentMode)
    {
    case ApplicationMode::ChooseTool:
        m_gestureRecognizer->TrySetGestureSettings(SpatialGestureSettings::Tap);
        m_gestureRecognizer->CaptureInteraction(interaction);
        break;
    }
}

void SpatialControllerHandler::OnSourceUpdated(SpatialInteractionSourceState^ state, SpatialCoordinateSystem^ coordinateSystem)
{
    // Update the source position
    __super::OnSourceUpdated(state, coordinateSystem);

    switch (m_currentMode)
    {
        case ApplicationMode::Drawing:
        {
            // Update the Pointing Pose
            bool insideTargetBoard = false;

            // Try to get the pointing ray and hit test the wall
            m_toolPosition.IsCurrent = false;
            SpatialPointerPose^ pointerPose = state->TryGetPointerPose(coordinateSystem);
            if (pointerPose != nullptr)
            {
                SpatialPointerInteractionSourcePose^ interactionSourcePose = pointerPose->TryGetInteractionSourcePose(state->Source);
                if (interactionSourcePose != nullptr)
                {
                    m_toolPosition.IsCurrent = true;
                    m_toolPosition.HasValue = true;
                    m_refreshModelConstantBufferData = true;

                    const float3& forwardDirection = interactionSourcePose->ForwardDirection;

                    // Try to hit test the drawing board
                    float targetDistance = 0.0f;
                    insideTargetBoard = m_targetBoard->Intersects(interactionSourcePose->Position, forwardDirection, &targetDistance);

                    if (!insideTargetBoard)
                    {
                        // Display and draw a few centimeters in front of the controller as we do not target the board
                        targetDistance = 0.05f;
                    }

                    m_toolPosition.Position = interactionSourcePose->Position + targetDistance * forwardDirection;

                    // Now compute the orientation of the tool using upDirection as well
                    // Compute the orientation based on the ray
                    const XMVECTOR orientationVector = DX::GetQuaternionVectorFromRay(forwardDirection, interactionSourcePose->UpDirection);
                    XMStoreQuaternion(&m_toolPosition.Orientation, orientationVector);
                }
            }
            m_toolPosition.CheckIfObsolete(state->Timestamp->TargetTime.UniversalTime);

            // If we just switched from inside the board to outside it and the controllers supports haptics, send a quick vibration
            if ((m_insideTargetBoard != insideTargetBoard) && (m_hapticsController != nullptr))
            {
                TimeSpan buzzDuration;
                buzzDuration.Duration = 1000000; // 1 == 100-nanosecond, therefore 0.1 second here...
                m_hapticsController->SendHapticFeedbackForDuration(m_hapticsBuzzFeedback, 1.0, buzzDuration);
            }

            if (state->IsSelectPressed)
            {
                if (!m_waitForRelease)
                {
                    if (!m_currentStroke)
                    {
                        // Start drawing
                        m_strokeInsideTargetBoard = m_insideTargetBoard;
                        ComposeStroke();
                    }
                    else if (m_strokeInsideTargetBoard == m_insideTargetBoard)
                    {
                        // Continue Drawing
                        ComposeStroke();
                    }
                    else
                    {
                        // Transition in/out the target, stop drawing
                        TerminateStroke();
                        // And wait for the button to be released to start a new Stroke
                        m_waitForRelease = true;
                    }
                }
            }
            else
            {
                m_waitForRelease = false;
                if (m_currentStroke)
                {
                    TerminateStroke();
                }
            }
        }
        break;
    case ApplicationMode::ChooseTool:
        {
            SpatialInteractionControllerProperties^ controllerProperties = (m_spDPadHelper ? state->ControllerProperties : nullptr);
            if (controllerProperties != nullptr)
            {
                DPadAction dpadAction = m_spDPadHelper->Update(controllerProperties);
                if (dpadAction != DPadAction::None)
                {
                    m_eventsHandler->OnDPadAction(this, dpadAction);
                }
            }
        }
        break;
    }
}

void SpatialControllerHandler::OnSourcePressed(SpatialInteractionSourceEventArgs^ args)
{
    SpatialInteractionSourceState^ state = args->State;
    SpatialInteractionControllerProperties^ controllerProperties = (m_spDPadHelper ? state->ControllerProperties : nullptr);
    if (controllerProperties != nullptr)
    {
        DPadAction dpadAction = m_spDPadHelper->SourcePressed(args->PressKind, controllerProperties);
        if (dpadAction != DPadAction::None)
        {
            m_eventsHandler->OnDPadAction(this, dpadAction);
        }
    }

    switch (m_currentMode)
    {
    case ApplicationMode::Drawing:
        switch (args->PressKind)
        {
        case SpatialInteractionPressKind::Menu:
            m_eventsHandler->OnEnterToolChoice(this, args->State->Timestamp);
            break;
        }
        break;
    case ApplicationMode::ChooseTool:
        switch (args->PressKind)
        {
        case SpatialInteractionPressKind::Menu:
            m_eventsHandler->OnSelectTool(this);
            break;
        }
        break;
    }
}

void SpatialControllerHandler::TryGetContinousBuzz(SimpleHapticsController^ controller)
{
    if ((controller != nullptr) && controller->IsPlayDurationSupported)
    {
        IVectorView<SimpleHapticsControllerFeedback^>^ feedbacks = controller->SupportedFeedback;
        // Only ContinousBuzz is supported and therefore we can use the first feedback of the collection
        if (feedbacks->Size > 0)
        {
            m_hapticsController = controller;
            m_hapticsBuzzFeedback = feedbacks->GetAt(0);
        }
    }
}

void SpatialControllerHandler::OnTapped(SpatialGestureRecognizer ^sender, SpatialTappedEventArgs ^args)
{
    switch (m_currentMode)
    {
    case ApplicationMode::ChooseTool:
        m_eventsHandler->OnSelectTool(this);
        break;
    }
}
