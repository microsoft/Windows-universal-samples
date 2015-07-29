// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "ShapeRenderer.h"

using namespace SDKTemplate;
using namespace Microsoft::WRL;

ShapeRenderer::ShapeRenderer()
{
    unsigned int seed = (unsigned int)time(0);
    srand(seed);

    _generator = std::default_random_engine(seed);
}

void ShapeRenderer::Render(Shape shape, Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext)
{
    d2dContext->FillRectangle(&shape.shape, shape.brush.Get());
}

void ShapeRenderer::CreateBrush(
    _Outptr_ ID2D1SolidColorBrush** brush, Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext)
{
    int R = rand() % 256;
    int G = rand() % 256;
    int B = rand() % 256;

    D2D1::ColorF d2dColor(R / 255.0f, G / 255.0f, B / 255.0f);
    *brush = nullptr;
    DX::ThrowIfFailed(
        d2dContext->CreateSolidColorBrush(
        d2dColor,
        brush
        )
        );
}

Shape ShapeRenderer::Update(Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext)
{
    Shape newShape;

    float centerX = _shapeDistributionX(_generator);
    float centerY = _shapeDistributionY(_generator);
    float width = _shapeDistributionX.mean() * 2;
    float height = _shapeDistributionY.mean() * 2;

    // The shape size is a percentage of the size of the canvas. This will create shapes that are proportional
    // to the canvas, regardless of the size of the screen.
    const float maxShapeSize = max(width, height) * MAX_SHAPE_SIZE_PERCENTAGE;
    const float minShapeSize = max(width, height) * MIN_SHAPE_SIZE_PERCENTAGE;

    float shapeWidth = 0, shapeHeight = 0;
    if (maxShapeSize > minShapeSize)
    {
        shapeWidth = (float)(rand() % ((int)(maxShapeSize - minShapeSize))) + minShapeSize;
        shapeHeight = (float)(rand() % ((int)(maxShapeSize - minShapeSize))) + minShapeSize;
    }

    float left = centerX - (shapeWidth / 2);
    float top = centerY - (shapeHeight / 2);

    // limit the shape position to the size of the canvas
    if (left <  0) left = maxShapeSize;
    if (top < 0) top = maxShapeSize;
    if (left > (width - maxShapeSize)) left = width - shapeWidth;
    if (top > (height - maxShapeSize))  top = height - shapeHeight;

    D2D1_RECT_F rectangle = D2D1::RectF(left, top, left + (shapeWidth / 2), top + (shapeHeight / 2));

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> shapeBrush;
    CreateBrush(&shapeBrush, d2dContext);

    newShape.shape = rectangle;
    newShape.brush = shapeBrush;

    return newShape;
}

void ShapeRenderer::RefreshSize(Windows::Foundation::Size newSize)
{
    int width = static_cast<int>(newSize.Width);
    int height = static_cast<int>(newSize.Height);

    if (width > 0 && height > 0)
    {
        // The mean will be the center of the viewport and the standard deviation a fourth of it
        _shapeDistributionX = std::normal_distribution<float>((float)width / 2, (float)width / 4);
        _shapeDistributionY = std::normal_distribution<float>((float)height / 2, (float)height / 4);
    }
}
