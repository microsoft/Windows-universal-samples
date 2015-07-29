// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include <inkrenderer.h>
#include "Common/DirectXHelper.h"
#include "WindowsNumerics.h"

namespace SDKTemplate
{
    class InkRenderer
    {
    public:
        InkRenderer();
        void DrawSelectionRect(Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext,
            Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory);
        void Render(Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^,
                    Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext);
        void Select(Windows::Foundation::Collections::IVector<Windows::Foundation::Point>^ selectionPoints);
        void UnselectAll();
        void Update(Windows::UI::Input::Inking::InkStroke^ strokeContainer);
        bool HasStrokesToErase(Windows::Foundation::Point prevPt, Windows::Foundation::Point currPt);
        Windows::Foundation::Rect DeleteSelectedStrokes();

        // InkRenderer accessors
        Windows::Foundation::Rect GetSelectionRectangle() { return _selectionRect; };
        void UpdateSelectionRectangle(Windows::Foundation::Rect newSelectionRect);
        Windows::UI::Input::Inking::InkStrokeContainer^ GetStrokeContainer() { return _strokeContainer; };
        Windows::UI::Input::Inking::InkStrokeContainer^ GetStrokeContainerForTemp() { return _StrokeContainerForTemp; };
        Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^ GetSelectedStrokes();
        void Clear();
        void ClearStrokeContainerForTemp() {
            if (_StrokeContainerForTemp != nullptr)
            {
                _StrokeContainerForTemp->Clear();
            }
        };
        
        bool IsEmptyRect(Windows::Foundation::Rect& updateRect)
        {
            return updateRect.IsEmpty || (updateRect.X == 0 && updateRect.Y == 0 && updateRect.Width == 0 && updateRect.Height == 0);
        }

    private:
        // Ink members
        Windows::UI::Input::Inking::InkStrokeContainer^ _strokeContainer;         // container to hold strokes from user input
        Windows::UI::Input::Inking::InkStrokeContainer^ _StrokeContainerForTemp;  // container to hold cloned strokes to mimic the hollow effect in selection
        Microsoft::WRL::ComPtr<IInkD2DRenderer> _spInkD2DRenderer;
        Windows::Foundation::Rect _selectionRect;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> _strokeStyle;
    };
}