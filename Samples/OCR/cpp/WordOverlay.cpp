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
#include "WordOverlay.h"

using namespace SDKTemplate;

WordOverlay::WordOverlay(OcrWord^ ocrWord) :
    word(ocrWord),
    isPropertyChangedObserved(false)
{
    UpdateProps(word->BoundingRect);
}

void WordOverlay::Transform(ScaleTransform^ transform)
{
    // Scale word box bounding rect and update properties.
    UpdateProps(transform->TransformBounds(word->BoundingRect));
}

void WordOverlay::UpdateProps(Rect boundingBox)
{
    wordPosition = *(ref new Thickness(boundingBox.Left, boundingBox.Top, 0, 0));
    wordWidth = boundingBox.Width;
    wordHeight = boundingBox.Height;

    OnPropertyChanged("WordPosition");
    OnPropertyChanged("WordWidth");
    OnPropertyChanged("WordHeight");
}

/// <summary>
/// Notifies listeners that a property value has changed. 
/// </summary>
/// <param name="propertyName">Name of the property used to notify listeners.</param> 
void WordOverlay::OnPropertyChanged(Platform::String^ propertyName)
{
    if (isPropertyChangedObserved)
    {
        PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
    }
}

Binding^ WordOverlay::CreateWordPositionBinding()
{
    Binding^ positionBinding = ref new Binding();
    positionBinding->Path = ref new PropertyPath("WordPosition");
    positionBinding->Source = this;

    return positionBinding;
}

Binding^ WordOverlay::CreateWordWidthBinding()
{
    Binding^ widthBinding = ref new Binding();
    widthBinding->Path = ref new PropertyPath("WordWidth");
    widthBinding->Source = this;

    return widthBinding;
}

Binding^ WordOverlay::CreateWordHeightBinding()
{
    Binding^ heightBinding = ref new Binding();
    heightBinding->Path = ref new PropertyPath("WordHeight");
    heightBinding->Source = this;

    return heightBinding;
}