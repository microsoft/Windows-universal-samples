// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "..\Common\DeviceResources.h"
#include "InkRenderer.h"
#include "ShapeRenderer.h"
#include "WindowsNumerics.h"

#define RECT_DELTA  3   // Inflate the invalidate rect to cover borderlines

namespace SDKTemplate 
{
    class SceneComposer : public IVirtualSurfaceUpdatesCallbackNative
    {
    public:
        SceneComposer() {};
        ~SceneComposer();
        Windows::UI::Xaml::Media::Imaging::VirtualSurfaceImageSource^ Initialize(
            Windows::Foundation::Size sizeContent,
            Windows::Foundation::Size sizeViewPort);
        // Adds strokes to the collection and calls the renderer to display the new scene
        void UpdateInk(Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ strokes);
        // Adds shapes to the collection and calls the renderer to display the new scene
        void UpdateShape();
        // Invalidate the area to be redrawn
        void Invalidate(_In_opt_ RECT* pInvalidateRect = nullptr);
        // Selects the strokes and call the renderer to update with new drawing attributes
        void SelectSceneObjects(Windows::Foundation::Collections::IVector<Windows::Foundation::Point>^ selectionPoints);
        // Updates the size of the device resources and updates the scene
        void OnViewSizeChanged(Windows::Foundation::Size newSize);
        // Unselects all strokes in the scene
        void UnselectSceneObjects();
        // Destroy cache of objects and clear the renderers
        void ClearScene();
        // Creates the resources for drawing the lasso
        void StartLassoSelection(Windows::Foundation::Point pt);
        // Render the lasso rectangle that will select strokes
        void DrawLasso(Windows::Foundation::Point pt);
        // Erases the stroke that cross the line from prevPt to currPt
        void DoErase(Windows::Foundation::Point prevPt, Windows::Foundation::Point currPt);
        // Set the current context window's content size
        void SetContentSize(Windows::Foundation::Size contentSize) {
            if (_deviceResources != nullptr) {
                _deviceResources->SetContentSize(contentSize);
            };
        };
        // Set the current context window's DPI value
        void SetDPI(float dpi) { 
            if (_deviceResources != nullptr) { 
                _deviceResources->SetDPI(dpi);
            };
        };
        // Get the current context window's DPI value
        float GetDPI() { return (_deviceResources != nullptr) ? _deviceResources->GetDPI() : 0; };
        // Set the current context window's content zoom factor
        void SetContentZoomFactor(float contentZoomFactor) {
            if (_deviceResources != nullptr) {
                _deviceResources->SetContentZoomFactor(contentZoomFactor);
            }
        };
        void Trim() {
            if (_deviceResources != nullptr) {
                _deviceResources->Trim();
            };
        };

        void CopySelected();
        void CutSelected();
        void PasteSelected(Windows::Foundation::Point pastePosition);
        
        // Scene accessors
        Windows::Foundation::Rect GetSceneSelectionRect();
        bool CanPaste() { return _inkRenderer->GetStrokeContainer()->CanPasteFromClipboard(); };

        // Ink Synchronizer
        void SetInkSynchronizer(Windows::UI::Input::Inking::InkSynchronizer^ inkSynchronizer) {
            _inkSynchronizer = inkSynchronizer;
        };
        
        void DoCustomDry();
        
        // IVirtualSurfaceUpdatesCallbackNative
        virtual HRESULT STDMETHODCALLTYPE UpdatesNeeded() override;

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID uuid,
            _Outptr_ void** object
            ) override;

        virtual ULONG STDMETHODCALLTYPE AddRef() override;

        virtual ULONG STDMETHODCALLTYPE Release() override;

    private:

        // Scene object is the base class for ink and shape objects

        enum SCENEOBJECT_TYPES {
            SOT_Ink = 0, SOT_Shape = 1, SOT_Generic = 2
        };

        class SceneObject
        {
        public:
            virtual SCENEOBJECT_TYPES getType() { return SCENEOBJECT_TYPES::SOT_Generic; };
        };

        class InkObject : public SceneObject
        {
        public:
            static float GetSelectedStrokeSize()
            {
                static const float selectedStrokeSize = 5.0f;
                return selectedStrokeSize;
            }

            static Windows::Foundation::Size GetDefaultStrokeSize()
            {
                static const Windows::Foundation::Size defaultStrokeSize = Windows::Foundation::Size(2.0f, 2.0f);
                return defaultStrokeSize;
            }

            // Constructor - set the drawing attributes for the custom dried stroke
            InkObject(Windows::UI::Input::Inking::InkStroke^ s) :_stroke(s) 
            {
                _highlighterStroke = nullptr;
            };

            virtual SCENEOBJECT_TYPES getType() { return SCENEOBJECT_TYPES::SOT_Ink; };

            bool IsStrokeSelected() { return _stroke->Selected; };
            Windows::UI::Input::Inking::InkStroke^ GetInkStroke() { return _stroke; };

            // Retrieve the stroke with highlighter (for the "hollow" effect) if selected
            Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^  getStrokes()
            { 
                Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>^ strokes = ref new Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>();
                strokes->Append(_stroke);
                if (_stroke->Selected)
                {
                    strokes->Append(_highlighterStroke);
                }
                return strokes; 
            };

            // If selected, we clone the stroke to create a highlighted stroke that will be on top of the original
            // to give a hollow effect
            void SelectStroke(Windows::UI::Input::Inking::InkStrokeContainer^ strokeContainerForTemp) {
                if (_stroke != nullptr)
                {
                    // Get the drawing attributes to modify them according to its state (inking or selection mode)
                    auto strokeDrawingAttributes = _stroke->DrawingAttributes;

                    if (_stroke->Selected)
                    {
                        // Create the highlighter stroke to give it a hollow effect
                        _highlighterStroke = _stroke->Clone();
                        _highlighterStroke->Selected = true;
                        auto highlighterAttributes = _highlighterStroke->DrawingAttributes;
                        highlighterAttributes->Color = CanvasBackgroundColor;
                        highlighterAttributes->Size = GetDefaultStrokeSize();
                        _highlighterStroke->DrawingAttributes = highlighterAttributes;

                        // Note that the Clone method must be used with AddStroke (InkManager) or AddStroke (InkStrokeContainer)
                        assert(strokeContainerForTemp != nullptr);
                        strokeContainerForTemp->AddStroke(_highlighterStroke);

                        strokeDrawingAttributes->Size = Windows::Foundation::Size(GetSelectedStrokeSize(), GetSelectedStrokeSize());
                        _stroke->DrawingAttributes = strokeDrawingAttributes;
                    }
                    else
                    {
                        // reset the pointer to highlighter stroke and set the corresponding inkstroke to the default drawing attributes
                        if (_highlighterStroke != nullptr)
                        {
                            _highlighterStroke = nullptr;
                        }
                        strokeDrawingAttributes->Size = GetDefaultStrokeSize();
                        _stroke->DrawingAttributes = strokeDrawingAttributes;
                    }
                }
            };

        private:
            Windows::UI::Input::Inking::InkStroke^ _stroke;
            Windows::UI::Input::Inking::InkStroke^ _highlighterStroke;
        };

        class ShapeObject : public SceneObject
        {
        public:
            ShapeObject(Shape s) : _shape(s) {};
            virtual SCENEOBJECT_TYPES getType() { return SCENEOBJECT_TYPES::SOT_Shape; };
            Shape getShape() { return _shape; };
        private:
            Shape _shape;
        };

        // Renders the cache of objects
        void Render();
        void DeleteSceneObjects();
        void SelectInkSceneObject();
        void DeleteSelectedAndUpdate();
        void RemoveSelectedStrokesFromSceneObjects();
        void RestoreDefaultStrokeSize();
        RECT ConvertUpdateRect(Windows::Foundation::Rect const& updateRect);
        bool Draw(_In_opt_ const RECT* pUpdateRect = nullptr);
        
        // This vector will contain all objects in the canvas (ink and shapes)
        std::vector<SceneObject*> _sceneObjects;
        std::unique_ptr<DX::DeviceResources> _deviceResources;
        std::unique_ptr<InkRenderer> _inkRenderer;
        std::unique_ptr<ShapeRenderer> _shapeRenderer;

        // Initial point for lasso rectangle
        Windows::Foundation::Point _ptInitial;
        // Current end point for lasso rectangle
        Windows::Foundation::Point _ptCurrent;
        // Flag to indicate lasso selection is going on
        bool _inLassoSelection;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> _selectionBrush;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> _strokeStyle;

        Windows::UI::Input::Inking::InkSynchronizer^ _inkSynchronizer;
        bool _EndDryPending;

        ULONG _refCount;
    };
}