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
#include "Scenario8.xaml.h"
#include <math.h>      

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Input::Inking::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Navigation;

const float Scenario8::radius = 150.0f;
const float Scenario8::circleStrokeThickness = 2.0f;
const float Scenario8::penSize = 4.0f;

Scenario8::Scenario8()
{
    InitializeComponent();

    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;

    auto drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
    drawingAttributes->Size = Size(penSize, penSize);
    inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);

    coreWetStrokeUpdateSource = CoreWetStrokeUpdateSource::Create(inkCanvas->InkPresenter);
    coreWetStrokeUpdateSource->WetStrokeStarting += ref new TypedEventHandler<CoreWetStrokeUpdateSource^, CoreWetStrokeUpdateEventArgs^>(this, &Scenario8::CoreWetStrokeUpdateSource_WetStrokeStarting);
    coreWetStrokeUpdateSource->WetStrokeContinuing += ref new TypedEventHandler<CoreWetStrokeUpdateSource^, CoreWetStrokeUpdateEventArgs^>(this, &Scenario8::CoreWetStrokeUpdateSource_WetStrokeContinuing);
    coreWetStrokeUpdateSource->WetStrokeStopping += ref new TypedEventHandler<CoreWetStrokeUpdateSource^, CoreWetStrokeUpdateEventArgs^>(this, &Scenario8::CoreWetStrokeUpdateSource_WetStrokeStopping);
    coreWetStrokeUpdateSource->WetStrokeCompleted += ref new TypedEventHandler<CoreWetStrokeUpdateSource^, CoreWetStrokeUpdateEventArgs^>(this, &Scenario8::CoreWetStrokeUpdateSource_WetStrokeCompleted);
    coreWetStrokeUpdateSource->WetStrokeCanceled += ref new TypedEventHandler<CoreWetStrokeUpdateSource^, CoreWetStrokeUpdateEventArgs^>(this, &Scenario8::CoreWetStrokeUpdateSource_WetStrokeCanceled);
}

void Scenario8::InkStackPanel_Loaded(Object^ sender, RoutedEventArgs e)
{
    inkCanvas->Width = InkStackPanel->ActualWidth;
    inkCanvas->Height = InkStackPanel->ActualHeight;

    circleCenter = Point(static_cast<float>(inkCanvas->Width) / 2.0f, static_cast<float>(inkCanvas->Height) / 2.0f);
    InkCircle->Center = circleCenter;
    InkCircle->RadiusX = radius;
    InkCircle->RadiusY = radius;
    InkCirclePath->StrokeThickness = circleStrokeThickness;
}

void Scenario8::CoreWetStrokeUpdateSource_WetStrokeStarting(CoreWetStrokeUpdateSource^ sender, CoreWetStrokeUpdateEventArgs^ args)
{
    followCircle = disableFollowCircle ? false : FollowCircleTest(args);
    if (followCircle)
    {
        HandleFollowCircleInput(args);
    }
    else
    {
        disableFollowCircle = true;
        HandleRegularInput(args);
    }
}

void Scenario8::CoreWetStrokeUpdateSource_WetStrokeContinuing(CoreWetStrokeUpdateSource^ sender, CoreWetStrokeUpdateEventArgs^ args)
{
    if (followCircle)
    {
        HandleFollowCircleInput(args);
    }
    else
    {
        HandleRegularInput(args);
    }
}

void Scenario8::CoreWetStrokeUpdateSource_WetStrokeStopping(CoreWetStrokeUpdateSource^ sender, CoreWetStrokeUpdateEventArgs^ args)
{
    if (followCircle)
    {
        HandleFollowCircleInput(args);
    }
    else
    {
        HandleRegularInput(args);
    }

    disableFollowCircle = false;
}

void Scenario8::CoreWetStrokeUpdateSource_WetStrokeCompleted(CoreWetStrokeUpdateSource^ sender, CoreWetStrokeUpdateEventArgs^ args)
{
    followCircle = false;
    discontinueStroke = false;
}

void Scenario8::CoreWetStrokeUpdateSource_WetStrokeCanceled(CoreWetStrokeUpdateSource^ sender, CoreWetStrokeUpdateEventArgs^ args)
{
    followCircle = false;
    discontinueStroke = false;
    disableFollowCircle = false;
}

bool Scenario8::FollowCircleTest(CoreWetStrokeUpdateEventArgs^ args)
{
    auto position = args->NewInkPoints->GetAt(0)->Position;
    auto distanceFromCenter = DistanceFromCenter(position);
    return InCircleEdgeZone(distanceFromCenter);
}

void Scenario8::HandleFollowCircleInput(CoreWetStrokeUpdateEventArgs^ args)
{
    if (!discontinueStroke)
    {
        int size = static_cast<int>(args->NewInkPoints->Size);
        for (int i = 0; i < size; i++)
        {
            auto position = args->NewInkPoints->GetAt(i)->Position;
            auto distanceFromCenter = DistanceFromCenter(position);

            if (InCircleEdgeZone(distanceFromCenter))
            {
                // Calculate position of new InkPoint so that it lies on the edge of the circle
                auto radians = atan2(position.Y - circleCenter.Y, position.X - circleCenter.X);
                auto totalRadius = (radius + circleStrokeThickness / 2.0f + penSize / 2.0f);
                Point newPoint = Point((cos(radians) * totalRadius) + circleCenter.X, (sin(radians) * totalRadius) + circleCenter.Y);

                args->NewInkPoints->SetAt(i, ref new InkPoint(newPoint, 0.5f));
            }
            else
            {
                // InkPoint is not in the follow circle zone, add no more InkPoints to Stroke
                discontinueStroke = true;

                for (int j = args->NewInkPoints->Size - 1; j >= i; j--)
                {
                    args->NewInkPoints->RemoveAt(j);
                }
                break;
            }
        }
    }
    else
    {
        // Stroke has been discontinued, remove all InkPoints
        for (int j = args->NewInkPoints->Size - 1; j >= 0; j--)
        {
            args->NewInkPoints->RemoveAt(j);
        }
    }
}

void Scenario8::HandleRegularInput(CoreWetStrokeUpdateEventArgs^ args)
{
    int size = static_cast<int>(args->NewInkPoints->Size);
    for (int i = 0; i < size; i++)
    {
        auto position = args->NewInkPoints->GetAt(i)->Position;
        auto distanceFromCenter = DistanceFromCenter(position);

        if (distanceFromCenter < radius + circleStrokeThickness / 2 + penSize / 2)
        {
            // Ink input entered the circle. Handle this by removing remaining InkPoints
            // and completing the stroke.

            for (int j = args->NewInkPoints->Size - 1; j >= i; j--)
            {
                args->NewInkPoints->RemoveAt(j);
            }

            args->Disposition = CoreWetStrokeDisposition::Completed;
            break;
        }
    }
}

double Scenario8::DistanceFromCenter(Point position)
{
    return hypot(position.X - circleCenter.X, position.Y - circleCenter.Y);
}

bool Scenario8::InCircleEdgeZone(double distanceFromCenter)
{
    return (distanceFromCenter > radius * 0.8) && (distanceFromCenter < radius * 1.5);
}


