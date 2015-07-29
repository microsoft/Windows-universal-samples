// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "InkRenderer.h"

using namespace SDKTemplate;
using namespace Microsoft::WRL;
using namespace Windows::Foundation::Numerics;

InkRenderer::InkRenderer()
{
    _strokeContainer = ref new Windows::UI::Input::Inking::InkStrokeContainer();

    _selectionRect = Windows::Foundation::Rect::Empty;

    CoCreateInstance(__uuidof(InkD2DRenderer),
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&_spInkD2DRenderer));
}

void InkRenderer::Clear()
{
    if (_strokeContainer != nullptr)
    {
        _strokeContainer->Clear();
    }
    _selectionRect = Windows::Foundation::Rect::Empty;
}

void InkRenderer::DrawSelectionRect(Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext, Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory)
{
    if (!_selectionRect.IsEmpty  &&
        _selectionRect.Bottom > _selectionRect.Top &&
        _selectionRect.Right > _selectionRect.Left &&
        d2dContext != nullptr)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> shapeBrush;
        DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.2f), &shapeBrush));

        D2D1_RECT_F rectangle = D2D1::RectF(_selectionRect.Left, _selectionRect.Top, _selectionRect.Right, _selectionRect.Bottom);
        d2dContext->FillRectangle(&rectangle, shapeBrush.Get());
        
        shapeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 0.5f));
        d2dContext->DrawRectangle(&rectangle, shapeBrush.Get());
    }
}

void InkRenderer::Render(Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^ strokes,
                         Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext)
{
    HRESULT hr = S_OK;
    if (_spInkD2DRenderer != nullptr)
    {
        if (strokes != nullptr && strokes->Size > 0)
        {
            // Cast the stroke collection into IUnknown to call the Inkd2dRenderer
            ComPtr<IUnknown> spUnkStrokes = reinterpret_cast<IUnknown*>(reinterpret_cast<__abi_IUnknown*>(strokes));
            hr = _spInkD2DRenderer->Draw(d2dContext.Get(), spUnkStrokes.Get(), false);
            if (FAILED(hr))
            {
                DX::ThrowIfFailed(hr);
            }
        }
    }
}


void InkRenderer::Select(Windows::Foundation::Collections::IVector<Windows::Foundation::Point>^ selectionPoints)
{
    if (selectionPoints != nullptr && _strokeContainer != nullptr)
    {
        _selectionRect = _strokeContainer->SelectWithPolyLine(selectionPoints);

        if (_StrokeContainerForTemp == nullptr && !IsEmptyRect(_selectionRect))
        {
            // Create a stroke container to hold the temporarily created strokes for selection hollow effect
            _StrokeContainerForTemp = ref new Windows::UI::Input::Inking::InkStrokeContainer();
        }
    }
}

void InkRenderer::UnselectAll()
{
    if (_strokeContainer != nullptr)
    {
        Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ inkStrokes = _strokeContainer->GetStrokes();
        for (unsigned int i = 0; i < inkStrokes->Size; i++)
        {
            inkStrokes->GetAt(i)->Selected = false;
        }
    }
    _selectionRect = Windows::Foundation::Rect::Empty;
}

bool InkRenderer::HasStrokesToErase(Windows::Foundation::Point prevPt, Windows::Foundation::Point currPt)
{
    Windows::Foundation::Rect updateRect = Windows::Foundation::Rect::Empty;
    if (_strokeContainer != nullptr)
    {
        updateRect = _strokeContainer->SelectWithLine(prevPt, currPt);
    }

    return !IsEmptyRect(updateRect);
}

Windows::Foundation::Rect InkRenderer::DeleteSelectedStrokes()
{
    Windows::Foundation::Rect updateRect = Windows::Foundation::Rect::Empty;
    if (_strokeContainer != nullptr)
    {
        updateRect = _strokeContainer->DeleteSelected();
    }

    return updateRect;
}

Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^ InkRenderer::GetSelectedStrokes()
{
    Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ strokesInContainer = _strokeContainer->GetStrokes();
    Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^ strokes = ref new Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>();
    for (unsigned int index = 0; index < strokesInContainer->Size; index++)
    {
        if (strokesInContainer->GetAt(index)->Selected)
        {
            strokes->Append(strokesInContainer->GetAt(index));
        }
    }
    return strokes;
}

void InkRenderer::Update(Windows::UI::Input::Inking::InkStroke^ stroke)
{
    if (stroke != nullptr && _strokeContainer != nullptr)
    {
        _strokeContainer->AddStroke(stroke);
    }
}

void InkRenderer::UpdateSelectionRectangle(Windows::Foundation::Rect newSelectionRect)
{
    _selectionRect = newSelectionRect;
}