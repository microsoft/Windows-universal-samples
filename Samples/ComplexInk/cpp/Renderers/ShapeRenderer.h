// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Common/DirectXHelper.h"
#include <random>
#include <stdlib.h>
#include <time.h>

#define MAX_SHAPE_SIZE_PERCENTAGE       0.20f
#define MIN_SHAPE_SIZE_PERCENTAGE       0.08f

namespace SDKTemplate
{
    // Struct to store the rectangle and the color for each object
    struct Shape
    {
        D2D1_RECT_F shape;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    };

    class ShapeRenderer
    {
    public:
        ShapeRenderer();
        Shape Update(Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext);
        void Render(Shape shape, Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext);
        void RefreshSize(Windows::Foundation::Size newSize);

    private:
        void CreateBrush(ID2D1SolidColorBrush** brush, Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext);

        // Random engine generator to get the position of shapes on the canvas
        std::default_random_engine _generator;

        // We create two normal distribution members so that shapes are mostly
        // placed in the middle of the canvas
        std::normal_distribution<float> _shapeDistributionX;
        std::normal_distribution<float> _shapeDistributionY;
    };
}