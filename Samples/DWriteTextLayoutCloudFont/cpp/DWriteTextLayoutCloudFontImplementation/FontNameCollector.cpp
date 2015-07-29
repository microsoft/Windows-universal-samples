//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "FontNameCollector.h"
#include "DirectXHelper.h"

using namespace DWriteTextLayoutCloudFontImplementation;
using namespace Microsoft::WRL;


std::wstring FontNameCollector::GetFontNames()
{
    std::wstring result;
    for (auto& name : m_uniqueFontNames)
    {
        if (!result.empty())
        {
            result += L", ";
        }
        result += name;
    }

    return result;
}


void FontNameCollector::ClearNames()
{
    // Clear any names to allow the collector to be re-used with a different layout.
    m_uniqueFontNames.clear();
}


// This is the one significant IDWriteTextRenderer method that will get 
// called and that we'll use to find out what fonts have been used in 
// the text layout.
HRESULT STDMETHODCALLTYPE FontNameCollector::DrawGlyphRun(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    _In_ DWRITE_GLYPH_RUN const* glyphRun,
    _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
    _In_opt_ IUnknown* clientDrawingEffect
    )
{
    // Since this is a platform callback that doesn't expect exceptions,
    // we shouldn't leave any exceptions unhandled.
    try
    {
        // The glyph run has a font face, which is what we need to get the font
        // name for this run. Specifically, we want an IDWriteFontFace3 object.

        ComPtr<IDWriteFontFace3> fontFace3;
        DX::ThrowIfFailed(glyphRun->fontFace->QueryInterface(fontFace3.GetAddressOf()));

        ComPtr<IDWriteLocalizedStrings> familyNames;
        DX::ThrowIfFailed(fontFace3->GetFamilyNames(&familyNames));

        // There may be family names in multiple languages, and in a different
        // situation we might want to look for a string that matches the app's UI
        // language (using Windows::Globalization::ApplicationLanguages::Languages).
        // For our purposes here, though, we know that the fonts used will have
        // family names for en-US, and that's sufficient since that's what we used
        // when we created the text format used in the layout.

        UINT32 nameStringIndex = 0;
        BOOL exists = false;
        DX::ThrowIfFailed(familyNames->FindLocaleName(L"en-US", &nameStringIndex, &exists));

        // Given the fonts that will be used, exists should always be true. If for
        // some reason it's false, nameStringIndex will be UINT32_MAX. We want to 
        // make sure it's 0: If there's no en-US string, we'll just used the first 
        // string.)
        if (!exists)
        {
            nameStringIndex = 0;
        }

        // Need a buffer to get the string.
        std::wstring fontName;
        UINT32 length = 0;
        DX::ThrowIfFailed(familyNames->GetStringLength(nameStringIndex, &length));
        fontName.resize(length);

        // Now get the name and add to the set of names.
        DX::ThrowIfFailed(familyNames->GetString(nameStringIndex, &fontName[0], length + 1));
        m_uniqueFontNames.insert(fontName);

        return S_OK;
    }
    catch (Platform::Exception^ e)
    {
        return e->HResult;
    }
    catch (const std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
}


// IDWritePixelSnapping methods won't actually get used in this sample
// and require only simple implementations:

HRESULT STDMETHODCALLTYPE FontNameCollector::GetCurrentTransform(
    _In_opt_ void* clientDrawingContext,
    _Out_ DWRITE_MATRIX* transform
    )
{
    static const DWRITE_MATRIX identity = {
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
        };
    *transform = identity;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE FontNameCollector::GetPixelsPerDip(
    _In_opt_ void* clientDrawingContext,
    _Out_ FLOAT* pixelsPerDip
    )
{
    *pixelsPerDip = 1.0f;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE FontNameCollector::IsPixelSnappingDisabled(
    _In_opt_ void* clientDrawingContext,
    _Out_ BOOL* isDisabled
    )
{
    *isDisabled = false;
    return S_OK;
}


// IUnknown methods will be used and require implementations:

HRESULT STDMETHODCALLTYPE FontNameCollector::QueryInterface(
    REFIID riid,
    _COM_Outptr_ void **obj
    )
{
    *obj = nullptr;
    if (riid == __uuidof(IDWriteTextRenderer) || riid == __uuidof(IDWritePixelSnapping) || riid == __uuidof(IUnknown))
    {
        AddRef();
        *obj = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}


ULONG STDMETHODCALLTYPE FontNameCollector::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE FontNameCollector::Release()
{
    uint32 newCount = InterlockedDecrement(&m_refCount);
    if (newCount == 0)
        delete this;
    return newCount;
}
