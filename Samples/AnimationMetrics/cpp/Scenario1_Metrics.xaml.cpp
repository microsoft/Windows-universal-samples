// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_Metrics.xaml.h"
#include <sstream>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core::AnimationMetrics;

Scenario1_Metrics::Scenario1_Metrics() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

double TimeSpanToMilliseconds(Windows::Foundation::TimeSpan timeSpan)
{
    return timeSpan.Duration / 10000.0f;
}

void Scenario1_Metrics::DisplayMetrics(AnimationEffect effect, AnimationEffectTarget target)
{
    std::wostringstream ss;
    AnimationDescription^ animationDescription = ref new AnimationDescription(effect, target);
    ss << "Stagger delay = " << TimeSpanToMilliseconds(animationDescription->StaggerDelay) << "ms" << std::endl;
    ss << "Stagger delay factor = " << animationDescription->StaggerDelayFactor << std::endl;
    ss << "Delay limit = " << TimeSpanToMilliseconds(animationDescription->DelayLimit) << "ms" << std::endl;
    ss << "ZOrder = " << animationDescription->ZOrder << std::endl;
    ss << std::endl;
    auto animations = animationDescription->Animations;
    for (unsigned int i = 0; i < animations->Size; i++)
    {
        ss << "Animation #" << (i + 1) << ":" << std::endl;
        IPropertyAnimation^ animation = animations->GetAt(i);
        switch (animation->Type)
        {
        case PropertyAnimationType::Scale:
            {
                ScaleAnimation^ scale = dynamic_cast<ScaleAnimation^>(animation);
                ss << "Type = Scale" << std::endl;
                if (scale->InitialScaleX != nullptr)
                {
                    ss << "InitialScaleX = " << scale->InitialScaleX->Value << std::endl;
                }
                if (scale->InitialScaleY != nullptr)
                {
                    ss << "InitialScaleY = " << scale->InitialScaleY->Value << std::endl;
                }
                ss << "FinalScaleX = " << scale->FinalScaleX << std::endl;
                ss << "FinalScaleY = " << scale->FinalScaleY << std::endl;
                ss << "Origin = " << scale->NormalizedOrigin.X << ", " << scale->NormalizedOrigin.Y << std::endl;
            }
            break;
        case PropertyAnimationType::Translation:
                ss << "Type = Translation" << std::endl;
            break;
        case PropertyAnimationType::Opacity:
            {
                OpacityAnimation^ opacity = dynamic_cast<OpacityAnimation^>(animation);
                ss << "Type = Opacity" << std::endl;
                if (opacity->InitialOpacity != nullptr)
                {
                    ss << "InitialOpacity = " << opacity->InitialOpacity->Value << std::endl;
                }
                ss << "FinalOpacity = " << opacity->FinalOpacity << std::endl;
            }
            break;
        }
        ss << "Delay = " << TimeSpanToMilliseconds(animation->Delay) << "ms" << std::endl;
        ss << "Duration = " << TimeSpanToMilliseconds(animation->Duration) << "ms" << std::endl;
        ss << "Cubic Bezier control points" << std::endl;
        ss << "    X1 = " << animation->Control1.X << ", Y1 = " << animation->Control1.Y << std::endl;
        ss << "    X2 = " << animation->Control2.X << ", Y2 = " << animation->Control2.Y << std::endl;
        ss << std::endl;
    }
    std::wstring s = ss.str();
    auto str = ref new Platform::String(s.c_str());
    Metrics->Text = str;
}

void Scenario1_Metrics::Animations_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    ListBoxItem^ selectedListBoxItem = dynamic_cast<ListBoxItem^>(Animations->SelectedItem);
    if (selectedListBoxItem == AddToListAdded)
    {
        DisplayMetrics(AnimationEffect::AddToList, AnimationEffectTarget::Added);
    }
    else if (selectedListBoxItem == AddToListAffected)
    {
        DisplayMetrics(AnimationEffect::AddToList, AnimationEffectTarget::Affected);
    }
    else if (selectedListBoxItem == EnterPage)
    {
        DisplayMetrics(AnimationEffect::EnterPage, AnimationEffectTarget::Primary);
    }
}
