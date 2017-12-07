#pragma once
#include "StepTimer.h"
#include "DeviceResources.h"

#define FONT_SIZE 10.667f
#define BUTTON_SIZE 64

namespace DX
{
    // Margin in terms of left, top, right, bottom.
    // "Pushes" the element away from its sibling elements and away from boundaries of its parent container.
    // Does not change the intristic size of the element; the parent element will take the margin of its child elements into consideration during arrangement.
    struct ElementMargin
    {
        ElementMargin() = default;

        ElementMargin(float l, float t, float r, float b) :
            left(l),
            top(t),
            right(r),
            bottom(b)
        {
            _ASSERTE(left >= 0.0f);
            _ASSERTE(top >= 0.0f);
            _ASSERTE(right >= 0.0f);
            _ASSERTE(bottom >= 0.0f);
        }

        float left = 0.0f;
        float top = 0.0f;
        float right = 0.0f;
        float bottom = 0.0f;
    };

    class BaseElement
    {
    public:
        BaseElement();
        virtual ~BaseElement() = default;
        virtual void Initialize();
        virtual void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose);
        void TryRender();
        void SetIsVisible(bool visible);
        bool GetIsVisible() { return m_isVisible; };
        void SetElementSize(Windows::Foundation::Numerics::float2 size) { m_elementSize = size; }
        bool GetIsRenderPassNeeded() { return m_isRenderPassNeeded; };
        Windows::Foundation::Numerics::float2 GetElementSize() { return m_elementSize; };
        // NOTE: This translation is the translation of the 2D BaseElement from the texture's origin
        virtual void SetTranslation(Windows::Foundation::Numerics::float2 translation);
        void SetParentTransform(Windows::Foundation::Numerics::float2 translation);
        void AddChildElement(const std::shared_ptr<BaseElement>& childElement);
        void SetElementMargin(ElementMargin margin) { m_elementMargin = margin; };
        void SetIgnoreLayout(bool ignoreLayout) { m_ignoreLayout = ignoreLayout; }
        ElementMargin& GetElementMargin() { return m_elementMargin; };
        bool GetIgnoreLayout() { return m_ignoreLayout; };
        void SetLayoutHorizontal(bool isLayoutHorizontal) { m_isLayoutHorizontal = isLayoutHorizontal; }
        void SetBrush(ID2D1SolidColorBrush* brush);
        // Applies layout properties down the element tree.
        // ArrangeElements must be manually called any time:
        // - The element's layout properties have changed (layout direction, content alignment, etc.)
        // - Any child's layout properties are updated.
        // - Children elements are added or removed.
        virtual void ArrangeElements();

    protected:
        virtual void Render();

        bool m_isInitialized;
        bool m_transformRecalculationNeeded;
        bool m_isRenderPassNeeded;
        bool m_isVisible;
        bool m_ignoreLayout;

        Windows::Foundation::Numerics::float2 m_translation;
        Windows::Foundation::Numerics::float2 m_transformedTranslation;
        Windows::Foundation::Numerics::float2 m_parentTranslation;
        // Width and height dimensions
        Windows::Foundation::Numerics::float2 m_elementSize;
        D2D1::Matrix3x2F m_transformMatrix = D2D1::Matrix3x2F::Identity();
        bool m_isLayoutHorizontal = false;
        ElementMargin m_elementMargin;

        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::vector<std::shared_ptr<BaseElement>> m_childElements;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
        // Cached pointer to device resources.
    };

    // Manage focus across multiple Elements 
    class FocusableElement : public BaseElement
    {
    public:
        FocusableElement();
        virtual ~FocusableElement() = default;
        virtual void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose) override;
        virtual bool IsFocused() { return m_isFocused; };
        virtual void PerformAction() {};
        virtual void SetParentControlInformation(bool focused, Windows::Foundation::Numerics::float2 focusedPosition, UINT parentTextureWidth, UINT parentTextureHeight);
        virtual void FocusUpdate() {};
    protected:
        bool m_isFocused;
        bool m_isParentFocused;
        // If an Element needs to know where the intersection occurs relative to its coordinates, set m_isSubElementIntersectionInfoNeeded true
        // For example, the parentTexture might be 100x100 and your Element might be 50x50 centered on the texture and lets say the Focus is also the center of the texture.
        // For most Elements, simply knowing that the coordinate 50,50 is inside the Element and thus its focused. But, some Elements such as Sliders need to know
        // Where in its Element Coordinate system did the intersection occur (so you can adjust the seekbar). In this case, by turning this parameter on, it will store the
        // Element Coordinate Intersection into the below field. So sticking with the above example, it would store 25,25 since the Element is centered in the Parent Texture
        bool m_isFocusIntersectionCalculated;
        Windows::Foundation::Numerics::float2 m_elementCoordIntersectionPoint;
        UINT m_parentTextureWidth;
        UINT m_parentTextureHeight;
        Windows::Foundation::Numerics::float2 m_parentFocusedPosition;
    };

    class Text : public BaseElement
    {
    public:
        Text(std::wstring text,
            FLOAT fontSize,
            DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_ULTRA_BOLD);
        void SetText(std::wstring text);

    protected:
        virtual void Render() override;

    private:
        void CreateTextContext();
        std::wstring m_text;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
        Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
        DWRITE_FONT_WEIGHT m_fontWeight;
        float m_transformedFontSize;
    };

    class Line : public BaseElement
    {
    public:
        Line(FLOAT strokeThickness);
        void SetStartPoint(D2D1_POINT_2F& point);
        void SetEndPoint(D2D1_POINT_2F& point);

    protected:
        virtual void Render() override;

    private:
        FLOAT m_strokeThickness;
        D2D1_POINT_2F m_lineStart;
        D2D1_POINT_2F m_lineEnd;
    };

    class Ellipse : public BaseElement
    {
    public:
        Ellipse(FLOAT radiusX, FLOAT radiusY, FLOAT strokeThickness, D2D1::ColorF brushColor);
        virtual void Initialize() override;
        void SetRadius(FLOAT x, FLOAT y);

    protected:
        virtual void Render() override;

    private:
        D2D1::ColorF m_brushColor;
        FLOAT m_strokeThickness;
        FLOAT m_radiusX;
        FLOAT m_radiusY;
        D2D1_ELLIPSE m_ellipse;
    };

    class FilledEllipse : public BaseElement
    {
    public:
        FilledEllipse(FLOAT radiusX, FLOAT radiusY, D2D1::ColorF brushColor);
        virtual void Initialize() override;
        void SetRadius(FLOAT x, FLOAT y);

    protected:
        virtual void Render() override;

    private:
        D2D1::ColorF m_brushColor;
        FLOAT m_radiusX;
        FLOAT m_radiusY;
        D2D1_ELLIPSE m_ellipse;
    };

    class Rectangle : public BaseElement
    {
    public:
        Rectangle(FLOAT width, FLOAT height, FLOAT strokeThickness, D2D1::ColorF brushColor);
        virtual void Initialize() override;
        void SetRect(const D2D1_RECT_F& rect) { m_rect = rect; }
        FLOAT GetStrokeThickness() { return m_strokeThickness; };

    protected:
        virtual void Render() override;

    private:
        D2D1_RECT_F m_rect;
        FLOAT m_strokeThickness;
        D2D1::ColorF m_brushColor;
    };

    class FilledRectangle : public BaseElement
    {
    public:
        FilledRectangle(FLOAT width, FLOAT height, D2D1::ColorF brushColor);
        virtual void Initialize() override;
        void SetRect(const D2D1_RECT_F& rect) { m_rect = rect; }

    protected:
        virtual void Render() override;

    private:
        D2D1_RECT_F m_rect;
        D2D1::ColorF m_brushColor;
    };
}
