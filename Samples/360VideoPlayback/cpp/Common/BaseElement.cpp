#include "pch.h"
#include "BaseElement.h"
#include "DirectXHelper.h"
#include "..\360VideoPlaybackMain.h"

using namespace _360VideoPlayback;
using namespace D2D1;
using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

const float MAX_TEXT_LAYOUT_SIZE = 500;

BaseElement::BaseElement() :
    m_isInitialized(false),
    m_transformRecalculationNeeded(false),
    m_isRenderPassNeeded(true),
    m_isVisible(true),
    m_translation(0.0f, 0.0f),
    m_transformedTranslation(0.0f, 0.0f),
    m_parentTranslation(0.0f, 0.0f),
    m_elementSize(0.0f, 0.0f),
    m_ignoreLayout(true),
    m_brush(nullptr)
{
    m_deviceResources = _360VideoPlaybackMain::GetDeviceResources();
}
void BaseElement::ArrangeElements()
{
    float maxWidth = 0.0f;
    float maxHeight = 0.0f;
    float containerWidth = 0.0f;
    float containerHeight = 0.0f;

    // Arrange Children And Compute whole ContainerSize
    for (const auto& element : m_childElements)
    {
        if (!element->GetIgnoreLayout())
        {
            // Skip stuff that doesn't want to be stacked in its container (like the focus rect).
            continue;
        }

        maxWidth = max(maxWidth, element->GetElementSize().x);
        maxHeight = max(maxHeight, element->GetElementSize().y);

        if (m_isLayoutHorizontal)
        {
            containerWidth += element->GetElementSize().x + element->GetElementMargin().left + element->GetElementMargin().right;
            containerHeight = max(containerHeight, element->GetElementSize().y + element->GetElementMargin().top + element->GetElementMargin().bottom);
        }
        else
        {
            containerWidth = max(containerWidth, element->GetElementSize().x + element->GetElementMargin().left + element->GetElementMargin().right);
            containerHeight = max(containerHeight, element->GetElementSize().y + element->GetElementMargin().top + element->GetElementMargin().bottom);
        }
    }

    float minWidth = m_elementSize.x;
    float minHeight = m_elementSize.y;

    containerWidth = max(minWidth, containerWidth);
    containerHeight = max(minHeight, containerHeight);
    m_elementSize.x = containerWidth;
    m_elementSize.y = containerHeight;

    // Actually position the elements. We need to do this after calculating the container size so we can center the elements.
    float currentStackPosition = 0.0;
    for (const auto& element : m_childElements)
    {
        if (!element->GetIgnoreLayout())
        {
            // Skip stuff that doesn't want to be stacked in its container (like the focus rect).
            continue;
        }

        if (m_isLayoutHorizontal)
        {
            // Position element along the stacking axis
            float elementPositionX = currentStackPosition + element->GetElementMargin().left;

            // Center the element so the stacking axis bisects the element
            float elementPositionY = (containerHeight / 2.0f) - (element->GetElementSize().y / 2.0f);

            // Adjust centered position to respect top margin
            if (elementPositionY < element->GetElementMargin().top)
            {
                elementPositionY = element->GetElementMargin().top;
            }

            // Adjust centered position to respect bottom margin
            if (elementPositionY + element->GetElementSize().y + element->GetElementMargin().bottom > containerHeight)
            {
                elementPositionY = containerHeight - element->GetElementSize().y - element->GetElementMargin().bottom;
            }

            element->SetTranslation({ elementPositionX, elementPositionY });

            currentStackPosition += element->GetElementMargin().left + element->GetElementSize().x + element->GetElementMargin().right;
        }
        else
        {
            float elementPositionX = (containerWidth / 2.0f) - (element->GetElementSize().x / 2.0f);
            float elementPositionY = (containerHeight / 2.0f) - (element->GetElementSize().y / 2.0f);
            element->SetTranslation({ elementPositionX, elementPositionY });
        }
    }
}

void BaseElement::Initialize()
{
    for (const auto& child : m_childElements)
    {
        child->Initialize();
    }
    m_isInitialized = true;
}

void BaseElement::SetBrush(ID2D1SolidColorBrush* brush)
{
    m_brush = brush;
    m_isRenderPassNeeded = true;
}

void BaseElement::Update(const DX::StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    // If one of BaseElement's transforms have changed or its parent's transforms have been updated
    // we will recalculate the BaseElement's transforms and tell its children to update their transforms
    // as well.
    // NOTE: For BaseElement's with complex nested children we should try not to update the transforms too 
    // frequently because this can cause perf issues. (fonts, textures, and glyphs may need to be resized)
    if (m_transformRecalculationNeeded)
    {
        m_transformedTranslation = m_translation + m_parentTranslation;
        m_transformMatrix = D2D1::Matrix3x2F::Translation(m_transformedTranslation.x, m_transformedTranslation.y);

        // Update children with updated transforms
        for (const auto& child : m_childElements)
        {
            child->SetParentTransform(m_transformedTranslation);
        }

        m_transformRecalculationNeeded = false;
        m_isRenderPassNeeded = true;
    }

    if (!m_isInitialized)
    {
        Initialize();
    }

    for (const auto& child : m_childElements)
    {
        child->Update(timer, cameraPose);
        m_isRenderPassNeeded |= child->GetIsRenderPassNeeded();
    }
}

void BaseElement::TryRender()
{
    if (m_isVisible)
    {
        Render();

        for (const auto& child : m_childElements)
        {
            child->TryRender();
        }
    }

    m_isRenderPassNeeded = false;
}

void BaseElement::Render()
{
}

void BaseElement::SetTranslation(float2 translation)
{
    m_transformRecalculationNeeded |= m_translation != translation;
    m_translation = translation;
}

void BaseElement::SetParentTransform(float2 translation)
{
    m_transformRecalculationNeeded |= (m_parentTranslation != translation);
    m_parentTranslation = translation;
}

void BaseElement::AddChildElement(const std::shared_ptr<BaseElement>& childElement)
{
    childElement->SetParentTransform(m_transformedTranslation);
    m_childElements.push_back(childElement);
}

void BaseElement::SetIsVisible(bool visible)
{
    m_isRenderPassNeeded |= (visible != m_isVisible);
    m_isVisible = visible;
};

FocusableElement::FocusableElement() :
    BaseElement(),
    m_isFocused(false),
    m_isParentFocused(false),
    m_isFocusIntersectionCalculated(false),
    m_elementCoordIntersectionPoint({ 0.0f,0.0f }),
    m_parentTextureWidth(0),
    m_parentTextureHeight(0),
    m_parentFocusedPosition(float2(0.0f))
{
}

void FocusableElement::Update(const StepTimer& timer, SpatialPointerPose^ cameraPose)
{
    __super::Update(timer, cameraPose);
    if (m_isParentFocused)
    {
        bool isFocused = false;

        float2 minBounds = m_transformedTranslation;
        float2 maxBounds = m_transformedTranslation + m_elementSize;
        // The focused position is relative to the model size, so we need to scale it to the texture size
        float2 textureScaledParentFocusedPosition = float2(m_parentFocusedPosition.x * m_parentTextureWidth, m_parentFocusedPosition.y * m_parentTextureHeight);
        if (minBounds.x <= textureScaledParentFocusedPosition.x && minBounds.y <= textureScaledParentFocusedPosition.y &&
            textureScaledParentFocusedPosition.x <= maxBounds.x && textureScaledParentFocusedPosition.y <= maxBounds.y)
        {
            isFocused = true;
        }

        if (m_isFocused != isFocused)
        {
            m_isRenderPassNeeded = true; // If this is a change in focus, mark we need a render pass        
            m_isFocused = isFocused;
            FocusUpdate();
        }

        if (m_isFocused && m_isFocusIntersectionCalculated) // This is mainly used by Slider Elements, which is why its behind a flag
        {
            m_elementCoordIntersectionPoint = float2(
                textureScaledParentFocusedPosition.x - (m_parentTextureWidth - m_elementSize.x),
                textureScaledParentFocusedPosition.y - (m_parentTextureHeight - m_elementSize.y));
        }
    }
    else
    {
        m_isFocused = false;
    }
}

void FocusableElement::SetParentControlInformation(bool focused, Windows::Foundation::Numerics::float2 focusedPosition, UINT parentTextureWidth, UINT parentTextureHeight)
{
    m_isParentFocused = focused;
    if (!m_isParentFocused)
    {
        if (m_isFocused)
        {
            m_isFocused = false;
            FocusUpdate();
        }
    }
    m_parentFocusedPosition = focusedPosition;
    m_parentTextureWidth = parentTextureWidth;
    m_parentTextureHeight = parentTextureHeight;
}

Text::Text(std::wstring text, FLOAT fontSize, DWRITE_FONT_WEIGHT fontWeight) :
    m_transformedFontSize(fontSize),
    m_fontWeight(fontWeight)
{
    this->SetText(text);
}

void Text::Render()
{
    D2D1_SIZE_F renderTargetSize = m_deviceResources->GetD2DDeviceContext()->GetSize();
    D2D1_POINT_2F textOrigin{ 0.0, 0.0 };
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Translation(m_transformedTranslation.x, m_transformedTranslation.y));
    d2dContext->DrawTextLayout(textOrigin, m_textLayout.Get(), m_brush.Get());
    d2dContext->SetTransform(D2D1::IdentityMatrix());
}

void Text::CreateTextContext()
{
    DX::ThrowIfFailed(m_deviceResources->GetDWriteFactory()->CreateTextFormat(
        L"Segoe UI",
        nullptr,
        m_fontWeight,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_transformedFontSize,
        Windows::Globalization::ApplicationLanguages::Languages->GetAt(0)->Data(),
        &m_textFormat));

    m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    DX::ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    DX::ThrowIfFailed(m_deviceResources->GetDWriteFactory()->CreateTextLayout(
        m_text.data(),
        static_cast<UINT32>(m_text.size()),
        m_textFormat.Get(),
        MAX_TEXT_LAYOUT_SIZE,
        MAX_TEXT_LAYOUT_SIZE,
        &m_textLayout));

    DWRITE_TEXT_METRICS textMetrics;
    m_textLayout->GetMetrics(&textMetrics);
    m_elementSize = { textMetrics.width, textMetrics.height };
    m_isRenderPassNeeded = true;
}

void Text::SetText(std::wstring text)
{
    if (text != m_text)
    {
        m_text = text;
        this->CreateTextContext();
    }
}

Line::Line(FLOAT strokeThickness) :
    m_strokeThickness(strokeThickness)
{
    m_lineStart = D2D1::Point2F(0.0f, 0.0f);
    m_lineEnd = D2D1::Point2F(1.0f, 0.0f);
}

void Line::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    d2dContext->SetTransform(m_transformMatrix);
    d2dContext->DrawLine(m_lineStart, m_lineEnd, m_brush.Get(), m_strokeThickness);
    d2dContext->SetTransform(D2D1::IdentityMatrix());
}

void Line::SetStartPoint(D2D1_POINT_2F& point)
{
    m_transformRecalculationNeeded |= (point.x != m_lineStart.x || point.y != m_lineStart.y);
    m_lineStart = point;
}

void Line::SetEndPoint(D2D1_POINT_2F& point)
{
    m_transformRecalculationNeeded |= (point.x != m_lineEnd.x || point.y != m_lineEnd.y);
    m_lineEnd = point;
}

Ellipse::Ellipse(FLOAT radiusX, FLOAT radiusY, FLOAT strokeThickness, ColorF brushColor) :
    BaseElement(),
    m_radiusX(radiusX),
    m_radiusY(radiusY),
    m_strokeThickness(strokeThickness),
    m_brushColor(brushColor)
{
    this->SetRadius(radiusX, radiusY);
}

void Ellipse::Initialize()
{
    if (m_brush == nullptr)
    {
        const auto d2dcontext = m_deviceResources->GetD2DDeviceContext();
        DX::ThrowIfFailed(d2dcontext->CreateSolidColorBrush(
            m_brushColor,
            &m_brush));
    }
    __super::Initialize();
}

void Ellipse::SetRadius(FLOAT x, FLOAT y)
{
    m_radiusX = x; m_radiusY = y;
    m_elementSize = { m_radiusX * 2.0f, m_radiusY * 2.0f };
    m_ellipse = D2D1::Ellipse(D2D1::Point2F(m_radiusX, m_radiusY), m_radiusX, m_radiusY);
}

void Ellipse::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    d2dContext->SetTransform(m_transformMatrix);
    d2dContext->DrawEllipse(m_ellipse, m_brush.Get(), m_strokeThickness);
    d2dContext->SetTransform(D2D1::IdentityMatrix());
}

FilledEllipse::FilledEllipse(FLOAT radiusX, FLOAT radiusY, ColorF brushColor) :
    BaseElement(),
    m_radiusX(radiusX),
    m_radiusY(radiusY),
    m_brushColor(brushColor)
{
    this->SetRadius(radiusX, radiusY);
}

void FilledEllipse::SetRadius(FLOAT x, FLOAT y)
{
    m_radiusX = x; m_radiusY = y;
    m_elementSize = { m_radiusX * 2.0f, m_radiusY * 2.0f };
    m_ellipse = D2D1::Ellipse(D2D1::Point2F(m_radiusX, m_radiusY), m_radiusX, m_radiusY);
}

void FilledEllipse::Initialize()
{
    if (m_brush == nullptr)
    {
        const auto d2dcontext = m_deviceResources->GetD2DDeviceContext();
        d2dcontext->CreateSolidColorBrush(
            m_brushColor,
            &m_brush);
    }
    __super::Initialize();
}

void FilledEllipse::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    d2dContext->SetTransform(m_transformMatrix);
    d2dContext->FillEllipse(m_ellipse, m_brush.Get());
    d2dContext->SetTransform(D2D1::IdentityMatrix());
}

Rectangle::Rectangle(FLOAT width, FLOAT height, FLOAT strokeThickness, ColorF brushColor) :
    BaseElement(),
    m_rect(D2D1::RectF(0.0f, 0.0f, width, height)),
    m_strokeThickness(strokeThickness),
    m_brushColor(brushColor)
{
}

void Rectangle::Initialize()
{
    if (m_brush == nullptr)
    {
        const auto d2dcontext = m_deviceResources->GetD2DDeviceContext();
        DX::ThrowIfFailed(d2dcontext->CreateSolidColorBrush(
            m_brushColor,
            &m_brush));
    }

    __super::Initialize();
}

void Rectangle::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    d2dContext->SetTransform(m_transformMatrix);
    d2dContext->DrawRectangle(m_rect, m_brush.Get(), m_strokeThickness);
    d2dContext->SetTransform(D2D1::IdentityMatrix());
}

FilledRectangle::FilledRectangle(FLOAT width, FLOAT height, ColorF brushColor) :
    BaseElement(),
    m_rect(D2D1::RectF(0.0f, 0.0f, width, height)),
    m_brushColor(brushColor)
{
}

void FilledRectangle::Initialize()
{
    if (m_brush == nullptr)
    {
        const auto d2dcontext = m_deviceResources->GetD2DDeviceContext();
        d2dcontext->CreateSolidColorBrush(
            m_brushColor,
            &m_brush
        );
    }
    __super::Initialize();
}

void FilledRectangle::Render()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    d2dContext->SetTransform(m_transformMatrix);
    d2dContext->FillRectangle(m_rect, m_brush.Get());
    d2dContext->SetTransform(D2D1::IdentityMatrix());
}
