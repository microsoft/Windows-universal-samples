#include "pch.h"
#include "MediaTransportControls.h"
#include "..\Common\DirectXHelper.h"
#include "..\Common\Controls.h"
#include "VideoRenderer.h"
#include "..\Common\SeekbarElement.h"
#include "..\Common\MeshGeometry.h"

using namespace _360VideoPlayback;
using namespace DX;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

const float MAX_INACTIVE_TIME_TO_HIDE = 5.0f; //In seconds
const float3 EXITBUTTON_SCALE{ 0.25f, 0.25f, 0.025f };
const float3 EXITBUTTON_POSITION{ 0.0f, -0.2f, 0.0f };
const float3 PLAYPAUSE_SCALE{ 0.25f, 0.25f, 0.025f };
const float3 PLAYPAUSE_POSITION{ 0.0f,  0.1f, 0.0f };
const float3 SEEKBAR_POSITION{ 0.0f,  -0.08f, 0.0f };
const unsigned short PLAYPAUSE_TESSELATION = 24;
const UINT PLAYPAUSE_CONTAINER_SIZE = BUTTON_SIZE * 2;
const UINT PLAYPAUSE_TEXT_SIZE = PLAYPAUSE_CONTAINER_SIZE / 2;

MediaTransportControls::MediaTransportControls()
{
    m_isVisible = false;
    m_rotation = float4(0, 0, 0, 0);
    m_position = float3(0, 0, 0);
    m_shadersLoaded = false;
    m_transform = float4x4::identity();
}

void MediaTransportControls::Initialize()
{
    this->AddPlayPauseButton();
    this->AddSeekbar();
    this->AddExitButton();
}

void MediaTransportControls::AddPlayPauseButton()
{
    auto playPauseButtonElement = std::make_shared<PlayPauseButton>(PLAYPAUSE_CONTAINER_SIZE, PLAYPAUSE_TEXT_SIZE);
    playPauseButtonElement->Initialize();

    // Play/pause button control
    m_playPauseButtonControl = std::make_shared<BaseControl>(MeshGeometry::MakeTexturedCylinder(PLAYPAUSE_TESSELATION));
    m_playPauseButtonControl->SetScale(PLAYPAUSE_SCALE);
    m_playPauseButtonControl->SetPosition(PLAYPAUSE_POSITION);
    m_playPauseButtonControl->SnapInitialSizeToRootElement();
    m_playPauseButtonControl->SetRenderElement(playPauseButtonElement);
    m_playPauseButtonControl->Initialize();
    m_baseControls.push_back(m_playPauseButtonControl);
}

void MediaTransportControls::AddSeekbar()
{
    auto seekBarStripSize = float2(300.0f, 50.0f);

    auto seekbarElement = std::make_shared<SeekBarElement>(
        static_cast<float>(seekBarStripSize.x),
        static_cast<float>(seekBarStripSize.y),
        static_cast<float>(seekBarStripSize.x) * 0.05f, /*left/right padding*/
        static_cast<float>(seekBarStripSize.y) * 0.44f /*bottom/top padding*/);

    seekbarElement->Initialize();
    seekbarElement->SetTranslation(float2(0.0f, static_cast<FLOAT>(seekBarStripSize.y) - seekbarElement->GetElementSize().y));

    m_seekbarControl = std::make_shared<BaseControl>();
    m_seekbarControl->SetTextureWidth(static_cast<UINT>(seekBarStripSize.x));
    m_seekbarControl->SetTextureHeight(static_cast<UINT>(seekBarStripSize.y));
    m_seekbarControl->SetScale(float3(0.25f, 0.25f, 0.25f));
    m_seekbarControl->SetPosition(SEEKBAR_POSITION);
    m_seekbarControl->SetRenderElement(seekbarElement);
    m_seekbarControl->Initialize();

    m_baseControls.push_back(m_seekbarControl);
}

void MediaTransportControls::AddExitButton()
{
    // Exit button element
    auto exitButtonElement = std::make_shared<ExitButton>();
    exitButtonElement->ArrangeElements();
    exitButtonElement->Initialize();

    // Exit button control
    m_exitButtonControl = std::make_shared<BaseControl>();
    m_exitButtonControl->SetScale(EXITBUTTON_SCALE);
    m_exitButtonControl->SetPosition(EXITBUTTON_POSITION);
    m_exitButtonControl->SnapInitialSizeToRootElement();
    m_exitButtonControl->SetRenderElement(exitButtonElement);
    m_exitButtonControl->Initialize();

    m_baseControls.push_back(m_exitButtonControl);
}

void MediaTransportControls::Update(const StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    const XMMATRIX modelRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation));
    const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));
    const XMMATRIX localTransform = XMMatrixMultiply(modelRotation, modelTranslation);
    XMStoreFloat4x4(&m_transform, localTransform);

    if (m_isVisible)
    {
        if (this->IsFocused())
        {
            m_inactiveSecondsCounter = 0.0f;
        }
        else
        {
            m_inactiveSecondsCounter += static_cast<float>(timer.GetElapsedSeconds());
            if (m_inactiveSecondsCounter >= MAX_INACTIVE_TIME_TO_HIDE)
            {
                Hide();
            }
        }
    }

    // Update the  Controls
    for (const auto& control : m_baseControls)
    {
        control->SetParentTransform(m_transform);
        control->Update(timer, cameraPose);
    }
}

void MediaTransportControls::Render()
{
    if (m_isVisible && m_shadersLoaded)
    {
        for (const auto& control : m_baseControls)
        {
            control->Render();
        }
    }
}

void MediaTransportControls::PerformPressedAction()
{
    bool pressedFocusedElement = false;
    for (const auto& control : m_baseControls)
    {
        if (control->IsFocused())
        {
            control->PerformPressedAction();
            pressedFocusedElement = true;
            break;
        }
    }

    if (!pressedFocusedElement)
    {
        Hide();
    }
}

void MediaTransportControls::Show(SpatialPointerPose^ cameraPose, float distance)
{
    const float3 headPosition = cameraPose->Head->Position;
    const float3 headDirection = cameraPose->Head->ForwardDirection;
    m_position = headPosition + (distance * headDirection);
    m_rotation = ComputeLookAtQuaternion(m_position, cameraPose->Head->Position, float3(0.0f, 1.0f, 0.0f));
    m_isVisible = true;
    for (const auto& control : m_baseControls)
    {
        control->SetIsVisible(true);
    }
    m_inactiveSecondsCounter = 0.0f;
}

void MediaTransportControls::Hide()
{
    m_isVisible = false;
    for (const auto& control : m_baseControls)
    {
        control->SetIsVisible(false);
    }
    m_inactiveSecondsCounter = 0.0f;
}

void MediaTransportControls::ApplyShaders()
{
    for (const auto& control : m_baseControls)
    {
        control->ApplyShaders();
    }
    m_shadersLoaded = true;
}

bool MediaTransportControls::IsFocused()
{
    for (const auto& control : m_baseControls)
    {
        if (control->IsFocused())
        {
            return true;
        }
    }
    return false;
}
