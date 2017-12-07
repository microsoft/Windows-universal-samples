#include "pch.h"
#include "DPadHelper.h"

using namespace HolographicPaint;
using namespace Windows::UI::Input::Spatial;

static DPadAction Select(double x, double y, double centerMin, double centerMax)
{
    int dx = (x < centerMin) ? 0 : ((x < centerMax) ? 1 : 2);
    int dy = (y < centerMin) ? 0 : ((y < centerMax) ? 1 : 2);
    switch (dx)
    {
    case 0: // Left
        switch (dy)
        {
        case 0: // Lower left
            return (fabs(x) > fabs(y)) ? DPadAction::Left : DPadAction::Bottom;
        case 1: // Middle Left
        default:
            return DPadAction::Left;
        case 2: // Top Left
            return (fabs(x) > fabs(y)) ? DPadAction::Left : DPadAction::Bottom;
        }
    case 1: // Middle
    default:
        switch (dy)
        {
        case 0: // Bottom Middle
            return DPadAction::Bottom;
        case 1: // Middle
        default:
            return DPadAction::None;
        case 2: // Top Middle 
            return DPadAction::Top;
        }
    case 2: // Right
        switch (dy)
        {
        case 0: // Bottom Right
            return (fabs(x) > fabs(y)) ? DPadAction::Right : DPadAction::Top;
        case 1: // Middle Right
        default:
            return DPadAction::Right;
        case 2: // Top right
            return (fabs(x) > fabs(y)) ? DPadAction::Right : DPadAction::Top;
        }
    }
}

ThumbstickDPadHelper::ThumbstickDPadHelper(double threshold)
    : m_threshold(threshold), m_waitForCenter(false)
{
}

DPadAction ThumbstickDPadHelper::Update(SpatialInteractionControllerProperties^ controllerState)
{
    double x = controllerState->ThumbstickX;
    double y = controllerState->ThumbstickY;
    DPadAction ret = DPadAction::None;
    if (m_waitForCenter)
    {
        m_waitForCenter = (x < -m_threshold) || (x > m_threshold) || (y < -m_threshold) || (y > m_threshold);
    }
    else
    {
        ret = Select(x, y, -m_threshold, m_threshold);
        m_waitForCenter = (ret != DPadAction::None);
    }
    return ret;
}

TouchpadDPadHelper::TouchpadDPadHelper(double threshold)
    : m_threshold(threshold)
{
}

DPadAction TouchpadDPadHelper::SourcePressed(
    SpatialInteractionPressKind pressKind,
    SpatialInteractionControllerProperties^ controllerState)
{
    double x = controllerState->TouchpadX;
    double y = controllerState->TouchpadY;
    return Select(x, y, -m_threshold, m_threshold);
}
