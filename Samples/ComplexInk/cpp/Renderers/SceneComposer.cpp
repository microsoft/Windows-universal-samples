// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "SceneComposer.h"

using namespace SDKTemplate;
using namespace Microsoft::WRL;

Windows::UI::Xaml::Media::Imaging::VirtualSurfaceImageSource^ SceneComposer::Initialize(
    Windows::Foundation::Size sizeContent, Windows::Foundation::Size sizeViewPort)
{
    _deviceResources = std::make_unique<DX::DeviceResources>(sizeContent, sizeViewPort);
    _shapeRenderer = std::make_unique<ShapeRenderer>();
    _inkRenderer = std::make_unique<InkRenderer>();

    assert(_deviceResources != nullptr);
    assert(_shapeRenderer != nullptr);
    assert(_inkRenderer != nullptr);

    _shapeRenderer->RefreshSize(sizeViewPort);

    // Register image source's update callback so update can be made to it.
    Microsoft::WRL::ComPtr<IVirtualSurfaceImageSourceNative> imageSourceNative = _deviceResources->GetImageSourceNative();
    DX::ThrowIfFailed(imageSourceNative->RegisterForUpdatesNeeded(this));

    _refCount = 0;
    _EndDryPending = false;
    _inLassoSelection = false;

    return _deviceResources->GetImageSource();
}

SceneComposer::~SceneComposer()
{
    // Unregister image source's update callback
    ComPtr<IVirtualSurfaceImageSourceNative> imageSourceNative = _deviceResources->GetImageSourceNative();
    DX::ThrowIfFailed(imageSourceNative->RegisterForUpdatesNeeded(nullptr));

    DeleteSceneObjects();
}

void SceneComposer::UpdateInk(Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ strokes)
{
    Windows::Foundation::Rect strokesBoundingRect = Windows::Foundation::Rect::Empty;

    assert(strokes != nullptr);
    assert(_inkRenderer != nullptr);

    for (unsigned int i = 0; i < strokes->Size; i++)
    {
        Windows::UI::Input::Inking::InkStroke^ inkStroke = strokes->GetAt(i);
        if (strokesBoundingRect.IsEmpty)
        {
            strokesBoundingRect = inkStroke->BoundingRect;
        }
        else
        {
            strokesBoundingRect.Union(inkStroke->BoundingRect);
        }
        _inkRenderer->Update(inkStroke);
        InkObject* inkObject = new InkObject(inkStroke);
        _sceneObjects.push_back(inkObject);
    }

    if (strokes->Size > 0)
    {
        assert(!strokesBoundingRect.IsEmpty);
        assert(_deviceResources != nullptr);
        RECT updateRect = _deviceResources->AdjustUpdateRect(ConvertUpdateRect(strokesBoundingRect));
        Invalidate(&updateRect);
    }
    else
    {
        // Need to call EndDry to complete the drying process even if there is zero stroke returned
        if (_EndDryPending)
        {
            assert(_inkSynchronizer != nullptr);
            _inkSynchronizer->EndDry();
            _EndDryPending = false;
        }
    }
}

void SceneComposer::UpdateShape()
{
    Windows::Foundation::Rect shapeRect = Windows::Foundation::Rect::Empty;

    assert(_shapeRenderer != nullptr);
    assert(_deviceResources != nullptr);
    Shape shape = _shapeRenderer->Update(_deviceResources->GetD2DDeviceContext());
    shapeRect.X = shape.shape.left;
    shapeRect.Y = shape.shape.top;
    shapeRect.Width = shape.shape.right - shape.shape.left;
    shapeRect.Height = shape.shape.bottom - shape.shape.top;

    SceneComposer::ShapeObject* shapeObject = new ShapeObject(shape);
    _sceneObjects.push_back(shapeObject);

    assert(shapeRect.Width > 0 && shapeRect.Height > 0);
    assert(_deviceResources != nullptr);
    RECT updateRect = _deviceResources->AdjustUpdateRect(ConvertUpdateRect(shapeRect));
    Invalidate(&updateRect);
}

void SceneComposer::Invalidate(_In_opt_ RECT* pInvalidateRect)
{
    assert(_deviceResources != nullptr);

    if (pInvalidateRect != nullptr)
    {
        // Invalidate VSIS to trigger rendering update
        _deviceResources->InvalidateRect(*pInvalidateRect);
    }
    else
    {
        _deviceResources->InvalidateContentRect();
    }
}

void SceneComposer::ClearScene()
{
    assert(_inkRenderer != nullptr);
    assert(_deviceResources != nullptr);

    DeleteSceneObjects();
    _sceneObjects.clear();
    _inkRenderer->Clear();
    _deviceResources->InvalidateContentRect();
}

void SceneComposer::DeleteSceneObjects()
{
    for (unsigned int index = 0; index < _sceneObjects.size(); index++)
    {
        if (_sceneObjects[index] != nullptr)
        {
            delete _sceneObjects[index];
            _sceneObjects[index] = nullptr;
        }
    }
}

void SceneComposer::SelectSceneObjects(Windows::Foundation::Collections::IVector<Windows::Foundation::Point>^ selectionPoints)
{
    _inLassoSelection = false;    // End of lasso selection

    assert(_inkRenderer != nullptr);
    // We call the inkRenderer to select the strokes in the strokecontainer. Then retrieve the stroke container
    // and update the vector of objects in the scene
    _inkRenderer->Select(selectionPoints);
    SelectInkSceneObject();
}

void SceneComposer::OnViewSizeChanged(Windows::Foundation::Size newSize)
{
    // This method can be called at program launch. By then device resources and
    // renderers are not initialized so we need to have null check.
    if (_shapeRenderer != nullptr)
    {
        // The size of shapes are proportional to the size of screen, so call
        // the shape renderer to recalculate max and min size for shapes.
        _shapeRenderer->RefreshSize(newSize);
    }
    if (_deviceResources != nullptr)
    {
        _deviceResources->SetViewSize(newSize);
        Invalidate();
    }
}

void SDKTemplate::SceneComposer::UnselectSceneObjects()
{
    assert(_inkRenderer != nullptr);
    assert(_deviceResources != nullptr);

    Windows::Foundation::Rect selectRect = _inkRenderer->GetSelectionRectangle();
    if (!selectRect.IsEmpty)
    {
        _inkRenderer->UnselectAll();
        SelectInkSceneObject();
        // Since we have undone the selection, it is safe to clear all the
        // temporarily created strokes for selection hollow effect
        _inkRenderer->ClearStrokeContainerForTemp();
        RECT updateRect = _deviceResources->AdjustUpdateRect(ConvertUpdateRect(selectRect));
        _deviceResources->InvalidateRect(updateRect);
    }
}

Windows::Foundation::Rect SceneComposer::GetSceneSelectionRect()
{
    assert(_inkRenderer != nullptr);
    Windows::Foundation::Rect rect = _inkRenderer->GetSelectionRectangle();
    return rect;
}

void SceneComposer::DoErase(Windows::Foundation::Point prevPt, Windows::Foundation::Point currPt)
{
    assert(_inkRenderer != nullptr);
    if (_inkRenderer->HasStrokesToErase(prevPt, currPt))
    {
        DeleteSelectedAndUpdate();
    }
}

void SceneComposer::DeleteSelectedAndUpdate()
{
    RemoveSelectedStrokesFromSceneObjects();
    Windows::Foundation::Rect updateRect = _inkRenderer->DeleteSelectedStrokes();

    if (!_inkRenderer->IsEmptyRect(updateRect))
    {
        assert(_deviceResources != nullptr);
        RECT vsisUpdateRect = _deviceResources->AdjustUpdateRect(ConvertUpdateRect(updateRect));
        _deviceResources->InvalidateRect(vsisUpdateRect);
    }
}

void SceneComposer::RemoveSelectedStrokesFromSceneObjects()
{
    for (std::vector<SceneObject*>::iterator pos = _sceneObjects.begin(); pos != _sceneObjects.end(); )
    {
        if ((*pos)->getType() == SOT_Ink)
        {
            InkObject* inkObject = dynamic_cast<InkObject*>(*pos);

            if(inkObject->IsStrokeSelected())
            {
                pos = _sceneObjects.erase(pos);
                delete inkObject;
                continue;
            }
        }
        pos++;
    }
}

void SceneComposer::StartLassoSelection(Windows::Foundation::Point pt)
{
    _ptInitial = pt;
    _inLassoSelection = true;    // Start of lasso selection

    assert(_deviceResources != nullptr);
    DX::ThrowIfFailed(_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &_selectionBrush));

    DX::ThrowIfFailed(_deviceResources->GetD2DFactory()->CreateStrokeStyle(
        D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_FLAT,
        D2D1_CAP_STYLE_FLAT,
        D2D1_CAP_STYLE_FLAT,
        D2D1_LINE_JOIN_ROUND,
        10.0F,
        D2D1_DASH_STYLE_DASH,
        0.0F),
        NULL,
        0,
        &_strokeStyle));
}

void SceneComposer::DrawLasso(Windows::Foundation::Point pt)
{
    assert(_deviceResources != nullptr);

    // Lasso selection continues. Update the current end point of lasso rect
    _ptCurrent = pt;
    // Invalidate VSIS to trigger rendering update
    _deviceResources->InvalidateContentRect();
}

void SceneComposer::Render()
{
    auto strokes = ref new Platform::Collections::Vector<Windows::UI::Input::Inking::InkStroke^>;
    for (unsigned int index = 0; index < _sceneObjects.size(); index++)
    {
        if (_sceneObjects[index]->getType() == SOT_Ink)
        {
            InkObject* inkObject = dynamic_cast<InkObject*>(_sceneObjects[index]);
            if (inkObject != nullptr)
            {
                auto currentInkStroke = inkObject->getStrokes();
                // The max number of strokes in here is 2, one for the actual stroke and one for the highlighter
                for (unsigned int strokeIndex = 0; strokeIndex < currentInkStroke->Size; strokeIndex++)
                {
                    strokes->Append(currentInkStroke->GetAt(strokeIndex));
                }
                // Instead of passing one stroke at a time, we append all the strokes in the same z-order
                // to optimize the rendering of ink. Once we find that the next object in the scene is different
                // from ink or if we have reached the end of the vector, we pass the stroke collection to the 
                // ink renderer.
                if ((index == (_sceneObjects.size() - 1)) ||
                    _sceneObjects[index + 1]->getType() == SOT_Shape)
                {
                    _inkRenderer->Render(strokes, _deviceResources->GetD2DDeviceContext());
                    strokes->Clear();
                }
            }
        }
        else if (_sceneObjects[index]->getType() == SOT_Shape)
        {
            ShapeObject* shape = dynamic_cast<ShapeObject*>(_sceneObjects[index]);
            if (shape != nullptr)
            {
                _shapeRenderer->Render(shape->getShape(), _deviceResources->GetD2DDeviceContext());
            }
        }
    }
}

// Restore the width of selected strokes to default in case that they are copied/cut for pasting
void SceneComposer::RestoreDefaultStrokeSize()
{
    assert(_inkRenderer != nullptr);
    Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ inkStrokes = _inkRenderer->GetStrokeContainer()->GetStrokes();
    for (unsigned int i = 0; i < inkStrokes->Size; i++)
    {
        Windows::UI::Input::Inking::InkStroke^ inkStroke = inkStrokes->GetAt(i);
        if (inkStroke->Selected)
        {
            auto strokeDrawingAttributes = inkStroke->DrawingAttributes;
            strokeDrawingAttributes->Size = InkObject::GetDefaultStrokeSize();
            inkStroke->DrawingAttributes = strokeDrawingAttributes;
        }
    }
}

void SceneComposer::CopySelected()
{
    assert(_inkRenderer != nullptr);
    RestoreDefaultStrokeSize();
    _inkRenderer->GetStrokeContainer()->CopySelectedToClipboard();
}

void SceneComposer::CutSelected()
{
    assert(_inkRenderer != nullptr);
    RestoreDefaultStrokeSize();
    _inkRenderer->GetStrokeContainer()->CopySelectedToClipboard();

    // Delete the scene objects in the cache and clear the selection rect
    _inkRenderer->UpdateSelectionRectangle(Windows::Foundation::Rect::Empty);
    DeleteSelectedAndUpdate();
}

void SceneComposer::PasteSelected(Windows::Foundation::Point pastePosition)
{
    assert(_inkRenderer != nullptr);

    if (_inkRenderer->GetStrokeContainer()->CanPasteFromClipboard())
    {
        Windows::Foundation::Rect pasteRect = _inkRenderer->GetStrokeContainer()->PasteFromClipboard(pastePosition);
        Windows::Foundation::Collections::IVector<Windows::Foundation::Point>^ pasteRectVector;
        
        // Create a selection rectangle that will be used to select strokes in the strokecontainer and then
        // we will add them to the scene object collection
        pasteRectVector = ref new Platform::Collections::Vector<Windows::Foundation::Point>();
        pasteRectVector->Append(Windows::Foundation::Point(pasteRect.Left, pasteRect.Top));
        pasteRectVector->Append(Windows::Foundation::Point(pasteRect.Left, pasteRect.Bottom));
        pasteRectVector->Append(Windows::Foundation::Point(pasteRect.Right, pasteRect.Bottom));
        pasteRectVector->Append(Windows::Foundation::Point(pasteRect.Right, pasteRect.Top));

        _inkRenderer->Select(pasteRectVector);
        Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ inkStrokes = _inkRenderer->GetStrokeContainer()->GetStrokes();
        for (unsigned int i = 0; i < inkStrokes->Size; i++)
        {
            if (inkStrokes->GetAt(i)->Selected)
            {
                InkObject* inkObject = new InkObject(inkStrokes->GetAt(i));
                _sceneObjects.push_back(inkObject);
            }
        }
        UnselectSceneObjects();
    }
}

void SceneComposer::SelectInkSceneObject()
{
    Windows::Foundation::Rect selectionBoundingRect = Windows::Foundation::Rect::Empty;

    // Update the sceneInkObject to create or delete the highlighter stroke
    for (unsigned int index = 0; index < _sceneObjects.size(); index++)
    {
        if (_sceneObjects[index]->getType() == SOT_Ink)
        {
            InkObject* inkObject = dynamic_cast<InkObject*>(_sceneObjects[index]);
            if (inkObject != nullptr)
            {
                assert(_inkRenderer != nullptr);
                inkObject->SelectStroke(_inkRenderer->GetStrokeContainerForTemp());

                // Need to update selection bounding rect since we have changed the size 
                // of selected strokes in the last step.
                if (inkObject->IsStrokeSelected())
                {
                    if (selectionBoundingRect.IsEmpty)
                    {
                        selectionBoundingRect = inkObject->GetInkStroke()->BoundingRect;
                    }
                    else
                    {
                        selectionBoundingRect.Union(inkObject->GetInkStroke()->BoundingRect);
                    }
                }
            }
        }
    }

    assert(_inkRenderer != nullptr);
    if (!_inkRenderer->IsEmptyRect(selectionBoundingRect))
    {
        _inkRenderer->UpdateSelectionRectangle(selectionBoundingRect);
    }
}

void SceneComposer::DoCustomDry()
{
    assert(_inkSynchronizer != nullptr);
    Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ inkStrokes = _inkSynchronizer->BeginDry();
    assert(inkStrokes != nullptr);
    // Request VSIS update by invalidation and set the flag to call
    // InkSynchronizer's EndDry later. Note that before EndDry is 
    // called, there will be no new StrokesCollected event fired
    // from InkPresenter, and we will not get any new stroke to dry.
    assert(!_EndDryPending);
    _EndDryPending = true;
    UpdateInk(inkStrokes);
}

// This method is called when the framework needs to update region managed by
// the virtual surface image source.
HRESULT STDMETHODCALLTYPE SceneComposer::UpdatesNeeded()
{
    HRESULT hr = S_OK;
    RECT* updateRects = nullptr;
    ULONG updateRectsCount = 0;

    assert(_deviceResources != nullptr);

    try
    {
        _deviceResources->GetUpdateRects(&updateRects, &updateRectsCount);

        for (ULONG i = 0; i < updateRectsCount; ++i)
        {
            Draw(&updateRects[i]);
        }

    }
    catch (Platform::Exception^ exception)
    {
        hr = exception->HResult;

        if (hr == D2DERR_RECREATE_TARGET ||
            hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            // HandleDeviceLost
            _deviceResources->CreateDeviceResources();

            _deviceResources->InvalidateContentRect();
        }
    }

    // Clear up buffer for updateRects
    if (updateRects != nullptr)
    {
        delete updateRects;
    }

    // Complete any pending call for EndDry so we can continue to
    // receive new strokes to dry through StrokesCollected event.
    if (_EndDryPending)
    {
        assert(_inkSynchronizer != nullptr);
        _inkSynchronizer->EndDry();
        _EndDryPending = false;
    }

    return hr;
}

bool SceneComposer::Draw(_In_opt_ const RECT* pUpdateRect)
{
    assert(_deviceResources != nullptr);

    _deviceResources->BeginDraw(pUpdateRect);

    _deviceResources->ClearTarget();

    Render();

    if (_inLassoSelection)
    {
        // Draw lasso
        assert(_selectionBrush != nullptr && _strokeStyle != nullptr);
        D2D_RECT_F selectionRect = D2D1::RectF(_ptInitial.X, _ptInitial.Y, _ptCurrent.X, _ptCurrent.Y);
        _deviceResources->GetD2DDeviceContext()->DrawRectangle(selectionRect, _selectionBrush.Get(), 1.0f, _strokeStyle.Get());
    }
    else
    {
        // Draw selection rectangle
        _inkRenderer->DrawSelectionRect(_deviceResources->GetD2DDeviceContext(),
            _deviceResources->GetD2DFactory());
    }

    _deviceResources->EndDraw();

    return false;
}

HRESULT STDMETHODCALLTYPE SceneComposer::QueryInterface(
    REFIID uuid,
    _Outptr_ void** object
    )
{
    if (uuid == IID_IUnknown
        || uuid == __uuidof(IVirtualSurfaceUpdatesCallbackNative)
        )
    {
        *object = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *object = nullptr;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE SceneComposer::AddRef()
{
    return static_cast<ULONG>(InterlockedIncrement(&_refCount));
}

ULONG STDMETHODCALLTYPE SceneComposer::Release()
{
    ULONG newCount = static_cast<ULONG>(InterlockedDecrement(&_refCount));

    if (newCount == 0)
        delete this;

    return newCount;
}

RECT SceneComposer::ConvertUpdateRect(Windows::Foundation::Rect const& updateRect)
{
    // Enlarge the update rect to accommodate precision loss when covert float to long
    RECT updateRECT;
    updateRECT.left = static_cast<LONG>(updateRect.X) - RECT_DELTA;
    updateRECT.top = static_cast<LONG>(updateRect.Y) - RECT_DELTA;
    updateRECT.right = static_cast<LONG>(updateRect.X + updateRect.Width) + RECT_DELTA;
    updateRECT.bottom = static_cast<LONG>(updateRect.Y + updateRect.Height) + RECT_DELTA;

    return updateRECT;
}