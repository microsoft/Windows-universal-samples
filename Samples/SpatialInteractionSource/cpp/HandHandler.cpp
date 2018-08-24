#include "pch.h"
#include "HandHandler.h"
#include "Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Devices::Haptics;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;

static const double NavigationThreshold = .3;

HandHandler::HandHandler(SpatialInteractionSource^ source, ISourceCommandHandler* eventsHandler, std::shared_ptr<Whiteboard>& target)
    : SourceHandler(source, eventsHandler, target)
{
    // Because Hands are Detected and Lost easily, we send all our feedback with a special SourceId
    m_sourceId = AnyHandSourceId;
    m_gestureRecognizer = ref new SpatialGestureRecognizer(SpatialGestureSettings::Tap | SpatialGestureSettings::ManipulationTranslate);
    // Handle Tapped gesture to switch mode
    m_tappedToken =
        m_gestureRecognizer->Tapped +=
            ref new TypedEventHandler<SpatialGestureRecognizer^, SpatialTappedEventArgs^>(
                std::bind(&HandHandler::OnTapped, this, _1, _2));

    // Handle Navigation gesture to switch tool
    m_navigationStartedToken =
        m_gestureRecognizer->NavigationStarted +=
            ref new TypedEventHandler<SpatialGestureRecognizer^, SpatialNavigationStartedEventArgs^>(
                std::bind(&HandHandler::OnNavigationStarted, this, _1, _2));
    m_navigationCompletedToken =
        m_gestureRecognizer->NavigationCompleted +=
            ref new TypedEventHandler<SpatialGestureRecognizer^, SpatialNavigationCompletedEventArgs^>(
                std::bind(&HandHandler::OnNavigationCompleted, this, _1, _2));

    // Handle Manipulation gesture to draw
    m_manipulationStartedToken =
        m_gestureRecognizer->ManipulationStarted +=
            ref new TypedEventHandler<SpatialGestureRecognizer ^, SpatialManipulationStartedEventArgs ^>(
                std::bind(&HandHandler::OnManipulationStarted, this, _1, _2));
    m_manipulationCanceledToken =
        m_gestureRecognizer->ManipulationCanceled +=
            ref new TypedEventHandler<SpatialGestureRecognizer ^, SpatialManipulationCanceledEventArgs ^>(
                std::bind(&HandHandler::OnManipulationCanceled, this, _1, _2));
    m_manipulationCompletedToken =
        m_gestureRecognizer->ManipulationCompleted +=
            ref new TypedEventHandler<SpatialGestureRecognizer ^, SpatialManipulationCompletedEventArgs ^>(
                std::bind(&HandHandler::OnManipulationCompleted, this, _1, _2));
    m_manipulationUpdatedToken =
        m_gestureRecognizer->ManipulationUpdated +=
            ref new TypedEventHandler<SpatialGestureRecognizer ^, SpatialManipulationUpdatedEventArgs ^>(
                std::bind(&HandHandler::OnManipulationUpdated, this, _1, _2));
}

HandHandler::~HandHandler()
{
    m_gestureRecognizer->Tapped -= m_tappedToken;
    m_gestureRecognizer->NavigationStarted -= m_navigationStartedToken;
    m_gestureRecognizer->NavigationCompleted -= m_navigationCompletedToken;
    m_gestureRecognizer->ManipulationStarted -= m_manipulationStartedToken;
    m_gestureRecognizer->ManipulationCanceled -= m_manipulationCanceledToken;
    m_gestureRecognizer->ManipulationCompleted -= m_manipulationCompletedToken;
    m_gestureRecognizer->ManipulationUpdated -= m_manipulationUpdatedToken;
}

void HandHandler::OnInteractionDetected(SpatialInteraction^ interaction, SpatialCoordinateSystem^ coordinateSystem)
{
    switch (m_currentMode)
    {
    case ApplicationMode::Drawing:
        m_gestureRecognizer->TrySetGestureSettings(SpatialGestureSettings::Tap | SpatialGestureSettings::ManipulationTranslate);
        break;
    case ApplicationMode::ChooseTool:
        m_gestureRecognizer->TrySetGestureSettings(SpatialGestureSettings::Tap | SpatialGestureSettings::NavigationX);
        break;
    }
    m_coordinateSystem = coordinateSystem;
    m_gestureRecognizer->CaptureInteraction(interaction);
}

void HandHandler::OnNavigationStarted(SpatialGestureRecognizer ^sender, SpatialNavigationStartedEventArgs ^args)
{
    HitTestTargetWithGaze(args->TryGetPointerPose(m_coordinateSystem));
    if (m_toolPosition.IsCurrent)
    {
        ComposeStroke();
    }
    else
    {
        // If the gaze is not hitting the board, cancel the current drawing operation
        m_gestureRecognizer->CancelPendingGestures();
    }
}

void HandHandler::OnManipulationStarted(SpatialGestureRecognizer ^sender, SpatialManipulationStartedEventArgs ^args)
{
    HitTestTargetWithGaze(args->TryGetPointerPose(m_coordinateSystem));
    if (m_toolPosition.IsCurrent)
    {
        m_initialPosition = m_toolPosition.Position;
        ComposeStroke();
    }
    else
    {
        // If the gaze is not hitting the board, cancel the current drawing operation
        m_gestureRecognizer->CancelPendingGestures();
    }
}


void HandHandler::OnManipulationCanceled(SpatialGestureRecognizer ^sender, SpatialManipulationCanceledEventArgs ^args)
{
    TerminateStroke();
}


void HandHandler::OnManipulationCompleted(SpatialGestureRecognizer ^sender, SpatialManipulationCompletedEventArgs ^args)
{
    TerminateStroke();
}


void HandHandler::OnManipulationUpdated(SpatialGestureRecognizer ^sender, SpatialManipulationUpdatedEventArgs ^args)
{
    auto delta = args->TryGetCumulativeDelta(m_coordinateSystem);
    auto translation = delta->Translation;
    translation.z = 0;
    m_toolPosition.Position = m_initialPosition + translation;
    if (m_targetBoard->IsValidPosition(m_toolPosition.Position))
    {
        m_refreshModelConstantBufferData = true;
        ComposeStroke();
    }
    else
    {
        m_gestureRecognizer->CancelPendingGestures();
    }
}

void HandHandler::OnNavigationCompleted(SpatialGestureRecognizer ^sender, SpatialNavigationCompletedEventArgs ^args)
{
    float3 delta = args->NormalizedOffset;
    DPadAction action = DPadAction::None;
    if (delta.x < -NavigationThreshold)
    {
        action = DPadAction::Left;
    }
    else if (delta.x > NavigationThreshold)
    {
        action = DPadAction::Right;
    }
    if (action != DPadAction::None)
    {
        m_eventsHandler->OnDPadAction(this, action);
    }
}

void HandHandler::OnTapped(SpatialGestureRecognizer ^sender, SpatialTappedEventArgs ^args)
{
    if (m_eventsHandler != nullptr)
    {
        switch (m_currentMode)
        {
        case ApplicationMode::ChooseTool:
            m_eventsHandler->OnSelectTool(this);
            break;
        case ApplicationMode::Drawing:
        {
            SpatialPointerPose^ pointerPose = args->TryGetPointerPose(m_coordinateSystem);
            if (pointerPose != nullptr)
            {
                m_eventsHandler->OnEnterToolChoice(this, pointerPose->Timestamp);
            }
        }
        break;
        }
    }
}


void HandHandler::OnSourceUpdated(SpatialInteractionSourceState^ state, SpatialCoordinateSystem^ coordinateSystem)
{
    // Update the source position
    __super::OnSourceUpdated(state, coordinateSystem);

    switch (m_currentMode)
    {
        case ApplicationMode::Drawing:
        {
            // If we are not already drawing, use Head Gaze to hit test the wall
            if (!m_currentStroke)
            {
                // Try to get the pointing ray and hit test the wall
                SpatialPointerPose^ pointerPose = state->TryGetPointerPose(coordinateSystem);
                if (pointerPose != nullptr)
                {
                    HitTestTargetWithGaze(pointerPose);

                    m_refreshModelConstantBufferData = m_toolPosition.IsCurrent;
                }
                m_toolPosition.CheckIfObsolete(state->Timestamp->TargetTime.UniversalTime);
            }
        }
        break;
    }
}

void HandHandler::HitTestTargetWithGaze(SpatialPointerPose^ pointerPose)
{
    m_toolPosition.IsCurrent = false;

    auto headPose = pointerPose->Head;
    const float3& forwardDirection = headPose->ForwardDirection;
    float targetDistance = 0.0f;

    if (m_targetBoard->Intersects(headPose->Position, forwardDirection, &targetDistance))
    {
        m_toolPosition.HasValue = true;
        m_toolPosition.IsCurrent = true;
        m_toolPosition.Position = headPose->Position + targetDistance * forwardDirection;

        // Compute the orientation based on the ray
        const XMVECTOR orientationVector = DX::GetQuaternionVectorFromRay(forwardDirection, headPose->UpDirection);
        XMStoreQuaternion(&m_toolPosition.Orientation, orientationVector);
    }
}


