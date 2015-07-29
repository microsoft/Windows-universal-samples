//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.cpp
//
// Functions for loading a DDS texture and creating a Direct3D 11 runtime resource for it
//
// Note these functions are useful as a light-weight runtime loader for DDS files. For
// a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#include "pch.h"
#include <dxgiformat.h>
#include <assert.h>
#include <memory>
#include <algorithm>
#include "DDSTextureLoader.h"
#include "DirectXSample.h"

using namespace Microsoft::WRL;

//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------
#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32)(byte)(ch0) | ((uint32)(byte)(ch1) << 8) |       \
                ((uint32)(byte)(ch2) << 16) | ((uint32)(byte)(ch3) << 24))
#endif /* defined(MAKEFOURCC) */

//--------------------------------------------------------------------------------------
// DDS file structure definitions
//
// See DDS.h in the 'Texconv' sample and the 'DirectXTex' library
//--------------------------------------------------------------------------------------
#pragma pack(push, 1)

#define DDS_MAGIC 0x20534444 // "DDS "

struct DDS_PIXELFORMAT
{
    uint32  size;
    uint32  flags;
    uint32  fourCC;
    uint32  RGBBitCount;
    uint32  RBitMask;
    uint32  GBitMask;
    uint32  BBitMask;
    uint32  ABitMask;
};

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_LUMINANCEA  0x00020001  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA
#define DDS_PAL8        0x00000020  // DDPF_PALETTEINDEXED8

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_WIDTH  0x00000004 // DDSD_WIDTH

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 // DDSCAPS_COMPLEX

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES (DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                              DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                              DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ)

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

enum DDS_MISC_FLAGS2
{
    DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7L,
};

enum DDS_ALPHA_MODE
{
    DDS_ALPHA_MODE_UNKNOWN       = 0,
    DDS_ALPHA_MODE_STRAIGHT      = 1,
    DDS_ALPHA_MODE_PREMULTIPLIED = 2,
    DDS_ALPHA_MODE_OPAQUE        = 3,
    DDS_ALPHA_MODE_CUSTOM        = 4,
};

typedef struct
{
    uint32          size;
    uint32          flags;
    uint32          height;
    uint32          width;
    uint32          pitchOrLinearSize;
    uint32          depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32          mipMapCount;
    uint32          reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32          caps;
    uint32          caps2;
    uint32          caps3;
    uint32          caps4;
    uint32          reserved2;
} DDS_HEADER;

typedef struct
{
    DXGI_FORMAT dxgiFormat;
    uint32      resourceDimension;
    uint32      miscFlag; // see D3D11_RESOURCE_MISC_FLAG
    uint32      arraySize;
    uint32      miscFlags2;
} DDS_HEADER_DXT10;

#pragma pack(pop)

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
static size_t BitsPerPixel(_In_ DXGI_FORMAT fmt)
{
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}


//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
static void GetSurfaceInfo(
    _In_ size_t width,
    _In_ size_t height,
    _In_ DXGI_FORMAT fmt,
    _Out_opt_ size_t* outNumBytes,
    _Out_opt_ size_t* outRowBytes,
    _Out_opt_ size_t* outNumRows
    )
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows = 0;

    bool bc = false;
    bool packed  = false;
    size_t bcnumBytesPerBlock = 0;
    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        bc = true;
        bcnumBytesPerBlock = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bcnumBytesPerBlock = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        packed = true;
        break;
    }

    if (bc)
    {
        size_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        }
        size_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        }
        rowBytes = numBlocksWide * bcnumBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ((width + 1) >> 1) * 4;
        numRows = height;
    }
    else
    {
        size_t bpp = BitsPerPixel(fmt);
        rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
        numRows = height;
    }

    numBytes = rowBytes * numRows;
    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }
    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }
    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}


//--------------------------------------------------------------------------------------
#define ISBITMASK(r, g, b, a) (ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a)

static DXGI_FORMAT GetDXGIFormat(const DDS_PIXELFORMAT& ddpf)
{
    if (ddpf.flags & DDS_RGB)
    {
        // Note that sRGB formats are written using the "DX10" extended header

        switch (ddpf.RGBBitCount)
        {
        case 32:
            if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
            {
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            }

            if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
            {
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            }

            if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
            {
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000) aka D3DFMT_X8B8G8R8

            // Note that many common DDS reader/writers (including D3DX) swap the
            // the RED/BLUE masks for 10:10:10:2 formats. We assumme
            // below that the 'backwards' header mask is being used since it is most
            // likely written by D3DX. The more robust solution is to use the 'DX10'
            // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

            // For 'correct' writers, this should be 0x000003ff, 0x000ffc00, 0x3ff00000 for RGB data
            if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
            {
                return DXGI_FORMAT_R10G10B10A2_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000) aka D3DFMT_A2R10G10B10

            if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
            {
                return DXGI_FORMAT_R16G16_UNORM;
            }

            if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
            {
                // Only 32-bit color channel format in D3D9 was R32F
                return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
            }
            break;

        case 24:
            // No 24bpp DXGI formats aka D3DFMT_R8G8B8
            break;

        case 16:
            if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
            {
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            }
            if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
            {
                return DXGI_FORMAT_B5G6R5_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x0000) aka D3DFMT_X1R5G5B5
            if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
            {
                return DXGI_FORMAT_B4G4R4A4_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x0f00, 0x00f0, 0x000f, 0x0000) aka D3DFMT_X4R4G4B4

            // No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
            break;
        }
    }
    else if (ddpf.flags & DDS_LUMINANCE)
    {
        if (8 == ddpf.RGBBitCount)
        {
            if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
            {
                return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // No DXGI format maps to ISBITMASK(0x0f, 0x00, 0x00, 0xf0) aka D3DFMT_A4L4
        }

        if (16 == ddpf.RGBBitCount)
        {
            if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
            {
                return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
            if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
            {
                return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
        }
    }
    else if (ddpf.flags & DDS_ALPHA)
    {
        if (8 == ddpf.RGBBitCount)
        {
            return DXGI_FORMAT_A8_UNORM;
        }
    }
    else if (ddpf.flags & DDS_FOURCC)
    {
        if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC1_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC3_UNORM;
        }

        // While pre-mulitplied alpha isn't directly supported by the DXGI formats,
        // they are basically the same as these BC formats so they can be mapped
        if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC3_UNORM;
        }

        if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_SNORM;
        }

        if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_SNORM;
        }

        // BC6H and BC7 are written using the "DX10" extended header

        if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
        {
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        }
        if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
        {
            return DXGI_FORMAT_G8R8_G8B8_UNORM;
        }

        // Check for D3DFORMAT enums being set here
        switch (ddpf.fourCC)
        {
        case 36: // D3DFMT_A16B16G16R16
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case 110: // D3DFMT_Q16W16V16U16
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case 111: // D3DFMT_R16F
            return DXGI_FORMAT_R16_FLOAT;

        case 112: // D3DFMT_G16R16F
            return DXGI_FORMAT_R16G16_FLOAT;

        case 113: // D3DFMT_A16B16G16R16F
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case 114: // D3DFMT_R32F
            return DXGI_FORMAT_R32_FLOAT;

        case 115: // D3DFMT_G32R32F
            return DXGI_FORMAT_R32G32_FLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}


//--------------------------------------------------------------------------------------
static DXGI_FORMAT MakeSRGB(_In_ DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    case DXGI_FORMAT_BC1_UNORM:
        return DXGI_FORMAT_BC1_UNORM_SRGB;

    case DXGI_FORMAT_BC2_UNORM:
        return DXGI_FORMAT_BC2_UNORM_SRGB;

    case DXGI_FORMAT_BC3_UNORM:
        return DXGI_FORMAT_BC3_UNORM_SRGB;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

    case DXGI_FORMAT_BC7_UNORM:
        return DXGI_FORMAT_BC7_UNORM_SRGB;

    default:
        return format;
    }
}


//--------------------------------------------------------------------------------------
static void FillInitData(
    _In_ size_t width,
    _In_ size_t height,
    _In_ size_t depth,
    _In_ size_t mipCount,
    _In_ size_t arraySize,
    _In_ DXGI_FORMAT format,
    _In_ size_t maxsize,
    _In_ size_t bitSize,
    _In_reads_bytes_(bitSize) const byte* bitData,
    _Out_ size_t& twidth,
    _Out_ size_t& theight,
    _Out_ size_t& tdepth,
    _Out_ size_t& skipMip,
    _Out_writes_(mipCount*arraySize) D3D11_SUBRESOURCE_DATA* initData
    )
{
    if (!bitData || !initData)
    {
        throw ref new Platform::InvalidArgumentException();
    }

    skipMip = 0;
    twidth = 0;
    theight = 0;
    tdepth = 0;

    size_t NumBytes = 0;
    size_t RowBytes = 0;
    size_t NumRows = 0;
    const byte* pSrcBits = bitData;
    const byte* pEndBits = bitData + bitSize;

    size_t index = 0;
    for (size_t j = 0; j < arraySize; j++)
    {
        size_t w = width;
        size_t h = height;
        size_t d = depth;
        for (size_t i = 0; i < mipCount; i++)
        {
            GetSurfaceInfo(w, h, format, &NumBytes, &RowBytes, &NumRows);

            if ((mipCount <= 1) || !maxsize || (w <= maxsize && h <= maxsize && d <= maxsize))
            {
                if (!twidth)
                {
                    twidth = w;
                    theight = h;
                    tdepth = d;
                }

                assert(index < mipCount * arraySize);
                _Analysis_assume_(index < mipCount * arraySize);
                initData[index].pSysMem = (const void*)pSrcBits;
                initData[index].SysMemPitch = static_cast<UINT>(RowBytes);
                initData[index].SysMemSlicePitch = static_cast<UINT>(NumBytes);
                ++index;
            }
            else if (!j)
            {
                // Count number of skipped mipmaps (first item only)
                ++skipMip;
            }

            if (pSrcBits + (NumBytes*d) > pEndBits)
            {
                throw ref new Platform::OutOfBoundsException();
            }

            pSrcBits += NumBytes * d;

            w = w >> 1;
            h = h >> 1;
            d = d >> 1;
            if (w == 0)
            {
                w = 1;
            }
            if (h == 0)
            {
                h = 1;
            }
            if (d == 0)
            {
                d = 1;
            }
        }
    }

    if (!index)
    {
        throw ref new Platform::FailureException();
    }
}


//--------------------------------------------------------------------------------------
static HRESULT CreateD3DResources(
    _In_ ID3D11Device* d3dDevice,
    _In_ uint32 resDim,
    _In_ size_t width,
    _In_ size_t height,
    _In_ size_t depth,
    _In_ size_t mipCount,
    _In_ size_t arraySize,
    _In_ DXGI_FORMAT format,
    _In_ D3D11_USAGE usage,
    _In_ unsigned int bindFlags,
    _In_ unsigned int cpuAccessFlags,
    _In_ unsigned int miscFlags,
    _In_ bool forceSRGB,
    _In_ bool isCubeMap,
    _In_reads_(mipCount*arraySize) D3D11_SUBRESOURCE_DATA* initData,
    _Outptr_opt_ ID3D11Resource** texture,
    _Outptr_opt_ ID3D11ShaderResourceView** textureView
    )
{
    if (!d3dDevice || !initData)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = E_FAIL;

    if (forceSRGB)
    {
        format = MakeSRGB(format);
    }

    switch (resDim)
    {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            {
                D3D11_TEXTURE1D_DESC desc;
                desc.Width = static_cast<UINT>(width);
                desc.MipLevels = static_cast<UINT>(mipCount);
                desc.ArraySize = static_cast<UINT>(arraySize);
                desc.Format = format;
                desc.Usage = usage;
                desc.BindFlags = bindFlags;
                desc.CPUAccessFlags = cpuAccessFlags;
                desc.MiscFlags = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;

                ID3D11Texture1D* tex = nullptr;
                hr = d3dDevice->CreateTexture1D(&desc, initData, &tex);

                if (SUCCEEDED(hr) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset(&SRVDesc, 0, sizeof(SRVDesc));
                        SRVDesc.Format = format;

                        if (arraySize > 1)
                        {
                            SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
                            SRVDesc.Texture1DArray.MipLevels = desc.MipLevels;
                            SRVDesc.Texture1DArray.ArraySize = static_cast<UINT>(arraySize);
                        }
                        else
                        {
                            SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
                            SRVDesc.Texture1D.MipLevels = desc.MipLevels;
                        }

                        hr = d3dDevice->CreateShaderResourceView(tex, &SRVDesc, textureView);

                        if (FAILED(hr))
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
                        tex->Release();
                    }
                }
            }
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            {
                D3D11_TEXTURE2D_DESC desc;
                desc.Width = static_cast<UINT>(width);
                desc.Height = static_cast<UINT>(height);
                desc.MipLevels = static_cast<UINT>(mipCount);
                desc.ArraySize = static_cast<UINT>(arraySize);
                desc.Format = format;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Usage = usage;
                desc.BindFlags = bindFlags;
                desc.CPUAccessFlags = cpuAccessFlags;
                if (isCubeMap)
                {
                    desc.MiscFlags = miscFlags | D3D11_RESOURCE_MISC_TEXTURECUBE;
                }
                else
                {
                    desc.MiscFlags = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;
                }

                ID3D11Texture2D* tex = nullptr;
                hr = d3dDevice->CreateTexture2D(&desc, initData, &tex);

                if (SUCCEEDED(hr) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset(&SRVDesc, 0, sizeof(SRVDesc));
                        SRVDesc.Format = format;

                        if (isCubeMap)
                        {
                            if (arraySize > 6)
                            {
                                SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
                                SRVDesc.TextureCubeArray.MipLevels = desc.MipLevels;

                                // Earlier we set arraySize to (NumCubes * 6)
                                SRVDesc.TextureCubeArray.NumCubes = static_cast<UINT>(arraySize / 6);
                            }
                            else
                            {
                                SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
                                SRVDesc.TextureCube.MipLevels = desc.MipLevels;
                            }
                        }
                        else if (arraySize > 1)
                        {
                            SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
                            SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
                            SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>(arraySize);
                        }
                        else
                        {
                            SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
                            SRVDesc.Texture2D.MipLevels = desc.MipLevels;
                        }

                        hr = d3dDevice->CreateShaderResourceView(tex, &SRVDesc, textureView);

                        if (FAILED(hr))
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
                        tex->Release();
                    }
                }
            }
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            {
                D3D11_TEXTURE3D_DESC desc;
                desc.Width = static_cast<UINT>(width);
                desc.Height = static_cast<UINT>(height);
                desc.Depth = static_cast<UINT>(depth);
                desc.MipLevels = static_cast<UINT>(mipCount);
                desc.Format = format;
                desc.Usage = usage;
                desc.BindFlags = bindFlags;
                desc.CPUAccessFlags = cpuAccessFlags;
                desc.MiscFlags = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;

                ID3D11Texture3D* tex = nullptr;
                hr = d3dDevice->CreateTexture3D(&desc, initData, &tex);

                if (SUCCEEDED(hr) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset(&SRVDesc, 0, sizeof(SRVDesc));
                        SRVDesc.Format = format;
                        SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
                        SRVDesc.Texture3D.MipLevels = desc.MipLevels;

                        hr = d3dDevice->CreateShaderResourceView(tex, &SRVDesc, textureView);

                        if (FAILED(hr))
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
                        tex->Release();
                    }
                }
            }
            break;
    }

    return hr;
}


//--------------------------------------------------------------------------------------
static void CreateTextureFromDDS(
    _In_ ID3D11Device* d3dDevice,
    _In_ const DDS_HEADER* header,
    _In_reads_bytes_(bitSize) const byte* bitData,
    _In_ size_t bitSize,
    _In_ size_t maxsize,
    _In_ D3D11_USAGE usage,
    _In_ unsigned int bindFlags,
    _In_ unsigned int cpuAccessFlags,
    _In_ unsigned int miscFlags,
    _In_ bool forceSRGB,
    _Outptr_opt_ ID3D11Resource** texture,
    _Outptr_opt_ ID3D11ShaderResourceView** textureView
    )
{
    HRESULT hr = S_OK;

    size_t width = header->width;
    size_t height = header->height;
    size_t depth = header->depth;

    uint32 resDim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    size_t arraySize = 1;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    bool isCubeMap = false;

    size_t mipCount = header->mipMapCount;
    if (0 == mipCount)
    {
        mipCount = 1;
    }

    if ((header->ddspf.flags & DDS_FOURCC) &&
        (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
    {
        auto d3d10ext = reinterpret_cast<const DDS_HEADER_DXT10*>((const char*)header + sizeof(DDS_HEADER));

        arraySize = d3d10ext->arraySize;
        if (arraySize == 0)
        {
            throw ref new Platform::FailureException();
        }

        if (BitsPerPixel(d3d10ext->dxgiFormat) == 0)
        {
            throw ref new Platform::FailureException();
        }

        format = d3d10ext->dxgiFormat;

        switch (d3d10ext->resourceDimension)
        {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            // D3DX writes 1D textures with a fixed Height of 1
            if ((header->flags & DDS_HEIGHT) && height != 1)
            {
                throw ref new Platform::FailureException();
            }
            height = depth = 1;
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            if (d3d10ext->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
            {
                arraySize *= 6;
                isCubeMap = true;
            }
            depth = 1;
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            if (!(header->flags & DDS_HEADER_FLAGS_VOLUME))
            {
                throw ref new Platform::FailureException();
            }

            if (arraySize > 1)
            {
                throw ref new Platform::FailureException();
            }
            break;

        default:
            return throw ref new Platform::FailureException();
        }

        resDim = d3d10ext->resourceDimension;
    }
    else
    {
        format = GetDXGIFormat(header->ddspf);

        if (format == DXGI_FORMAT_UNKNOWN)
        {
            return throw ref new Platform::FailureException();
        }

        if (header->flags & DDS_HEADER_FLAGS_VOLUME)
        {
            resDim = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
        }
        else
        {
            if (header->caps2 & DDS_CUBEMAP)
            {
                // We require all six faces to be defined
                if ((header->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                {
                    return throw ref new Platform::FailureException();
                }

                arraySize = 6;
                isCubeMap = true;
            }

            depth = 1;
            resDim = D3D11_RESOURCE_DIMENSION_TEXTURE2D;

            // Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
        }

        assert(BitsPerPixel(format) != 0);
    }

    // Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
    if (mipCount > D3D11_REQ_MIP_LEVELS)
    {
        return throw ref new Platform::FailureException();
    }

    switch (resDim)
    {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            if ((arraySize > D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION) ||
                (width > D3D11_REQ_TEXTURE1D_U_DIMENSION))
            {
                return throw ref new Platform::FailureException();
            }
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            if (isCubeMap)
            {
                // This is the right bound because we set arraySize to (NumCubes*6) above
                if ((arraySize > D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
                    (width > D3D11_REQ_TEXTURECUBE_DIMENSION) ||
                    (height > D3D11_REQ_TEXTURECUBE_DIMENSION))
                {
                    return throw ref new Platform::FailureException();
                }
            }
            else if ((arraySize > D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
                    (width > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION) ||
                    (height > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION))
            {
                return throw ref new Platform::FailureException();
            }
            break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            if ((arraySize > 1) ||
                (width > D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
                (height > D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
                (depth > D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION))
            {
                return throw ref new Platform::FailureException();
            }
            break;
    }

    // Create the texture
    std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> initData(new D3D11_SUBRESOURCE_DATA[mipCount * arraySize]);

    size_t skipMip = 0;
    size_t twidth = 0;
    size_t theight = 0;
    size_t tdepth = 0;
    FillInitData(width, height, depth, mipCount, arraySize, format, maxsize, bitSize, bitData, twidth, theight, tdepth, skipMip, initData.get());

    hr = CreateD3DResources(d3dDevice, resDim, twidth, theight, tdepth, mipCount - skipMip, arraySize, format, usage, bindFlags, cpuAccessFlags, miscFlags, forceSRGB, isCubeMap, initData.get(), texture, textureView);

    if (FAILED(hr) && !maxsize && (mipCount > 1))
    {
        // Retry with a maxsize determined by feature level
        switch (d3dDevice->GetFeatureLevel())
        {
        case D3D_FEATURE_LEVEL_9_1:
        case D3D_FEATURE_LEVEL_9_2:
            if (isCubeMap)
            {
                maxsize = D3D_FL9_1_REQ_TEXTURECUBE_DIMENSION;
            }
            else
            {
                maxsize = (resDim == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
                    ? D3D_FL9_1_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
                    : D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION;
            }
            break;

        case D3D_FEATURE_LEVEL_9_3:
            maxsize = (resDim == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
                ? D3D_FL9_1_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
                : D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION;
            break;

        default: // D3D_FEATURE_LEVEL_10_0 & D3D_FEATURE_LEVEL_10_1
            maxsize = (resDim == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
                ? D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
                : D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION;
            break;
        }

        FillInitData(width, height, depth, mipCount, arraySize, format, maxsize, bitSize, bitData, twidth, theight, tdepth, skipMip, initData.get());

        hr = CreateD3DResources(d3dDevice, resDim, twidth, theight, tdepth, mipCount - skipMip, arraySize, format, usage, bindFlags, cpuAccessFlags, miscFlags, forceSRGB, isCubeMap, initData.get(), texture, textureView);
    }

    DX::ThrowIfFailed(hr);
}


//--------------------------------------------------------------------------------------
static D2D1_ALPHA_MODE GetAlphaMode(_In_ const DDS_HEADER* header)
{
    if (header->ddspf.flags & DDS_FOURCC)
    {
        if (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC)
        {
            auto d3d10ext = reinterpret_cast<const DDS_HEADER_DXT10*>((const char*)header + sizeof(DDS_HEADER));
            switch (d3d10ext->miscFlags2 & DDS_MISC_FLAGS2_ALPHA_MODE_MASK)
            {
            case DDS_ALPHA_MODE_STRAIGHT:
                return D2D1_ALPHA_MODE_STRAIGHT;

            case DDS_ALPHA_MODE_PREMULTIPLIED:
                return D2D1_ALPHA_MODE_PREMULTIPLIED;

            case DDS_ALPHA_MODE_OPAQUE:
            case DDS_ALPHA_MODE_CUSTOM:
                // No D2D1_ALPHA_MODE equivalent, so return "Ignore" for now
                return D2D1_ALPHA_MODE_IGNORE;
            }
        }
        else if ((MAKEFOURCC('D', 'X', 'T', '2') == header->ddspf.fourCC)
                  || (MAKEFOURCC('D', 'X', 'T', '4') == header->ddspf.fourCC))
        {
            return D2D1_ALPHA_MODE_PREMULTIPLIED;
        }
        // DXT1, DXT3, and DXT5 legacy files could be straight alpha or something else, so return "Unknown" to leave it up to the app
    }

    return D2D1_ALPHA_MODE_UNKNOWN;
}


//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void CreateDDSTextureFromMemory(
    ID3D11Device* d3dDevice,
    const uint8_t* ddsData,
    size_t ddsDataSize,
    ID3D11Resource** texture,
    ID3D11ShaderResourceView** textureView,
    size_t maxsize,
    D2D1_ALPHA_MODE* alphaMode
    )
{
    return CreateDDSTextureFromMemoryEx(d3dDevice, ddsData, ddsDataSize, maxsize, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, texture, textureView, alphaMode);
}


//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void CreateDDSTextureFromMemoryEx(
    ID3D11Device* d3dDevice,
    const uint8_t* ddsData,
    size_t ddsDataSize,
    size_t maxsize,
    D3D11_USAGE usage,
    unsigned int bindFlags,
    unsigned int cpuAccessFlags,
    unsigned int miscFlags,
    bool forceSRGB,
    ID3D11Resource** texture,
    ID3D11ShaderResourceView** textureView,
    D2D1_ALPHA_MODE* alphaMode
    )
{
    if (texture)
    {
        *texture = nullptr;
    }
    if (textureView)
    {
        *textureView = nullptr;
    }
    if (alphaMode)
    {
        *alphaMode = D2D1_ALPHA_MODE_UNKNOWN;
    }

    if (!d3dDevice || !ddsData || (!texture && !textureView))
    {
        throw ref new Platform::InvalidArgumentException();
    }

    // Validate DDS file in memory
    if (ddsDataSize < (sizeof(uint32) + sizeof(DDS_HEADER)))
    {
        throw ref new Platform::FailureException();
    }

    uint32 dwMagicNumber = *(const uint32*)(ddsData);
    if (dwMagicNumber != DDS_MAGIC)
    {
        throw ref new Platform::FailureException();
    }

    auto header = reinterpret_cast<const DDS_HEADER*>(ddsData + sizeof(uint32));

    // Verify header to validate DDS file
    if (header->size != sizeof(DDS_HEADER) ||
        header->ddspf.size != sizeof(DDS_PIXELFORMAT))
    {
        throw ref new Platform::FailureException();
    }

    // Check for DX10 extension
    bool bDXT10Header = false;
    if ((header->ddspf.flags & DDS_FOURCC) &&
        (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
    {
        // Must be long enough for both headers and magic value
        if (ddsDataSize < (sizeof(DDS_HEADER) + sizeof(uint32) + sizeof(DDS_HEADER_DXT10)))
        {
            throw ref new Platform::FailureException();
        }

        bDXT10Header = true;
    }

    ptrdiff_t offset = sizeof(uint32) + sizeof(DDS_HEADER) + (bDXT10Header ? sizeof(DDS_HEADER_DXT10) : 0);

    CreateTextureFromDDS(d3dDevice, header, ddsData + offset, ddsDataSize - offset, maxsize, usage, bindFlags, cpuAccessFlags, miscFlags, forceSRGB, texture, textureView);

    if (alphaMode)
        *alphaMode = GetAlphaMode(header);
}
