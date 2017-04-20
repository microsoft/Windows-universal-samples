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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

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

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Select a Line Display", "SDKTemplate.Scenario1_SelectDisplay" },
    { "Display text", "SDKTemplate.Scenario2_DisplayText" },
};

#if 0
void SDKTemplate::MainPage::DisplayText(Platform::String^ text, Windows::Devices::PointOfService::LineDisplayTextAttribute attribute, bool middle)
{
    if (this->deviceId == nullptr)
    {
        NotifyUser("Select a Line Display first", NotifyType::ErrorMessage);
        return;
    }

    NotifyUser("Please wait...", NotifyType::StatusMessage);

    create_task(ClaimedLineDisplay::FromIdAsync(this->deviceId)).then([this, text, attribute, middle](ClaimedLineDisplay^ lineDisplay)
    {
        if (lineDisplay != nullptr)
        {
            Point position = {};
            if (middle)
            {
                uint32 length = text->Length();
                if (length < lineDisplay->DefaultWindow->SizeInCharacters.Width)
                {
                    position.X = (lineDisplay->DefaultWindow->SizeInCharacters.Width - length) / 2;
                }
            }

            Windows::Devices::PointOfService::LineDisplayTextAttribute _attribute = attribute;
            if (_attribute == LineDisplayTextAttribute::Blink)
            {
                if (lineDisplay->Capabilities->CanBlink == LineDisplayTextAttributeGranularity::NotSupported)
                {
                    _attribute = LineDisplayTextAttribute::Normal;
                }
            }

            create_task(lineDisplay->DefaultWindow->TryClearTextAsync()).then([this, text, _attribute, position, lineDisplay](bool result)
            {
                create_task(lineDisplay->DefaultWindow->TryDisplayTextAsync(text, _attribute, position)).then([this](bool result)
                {
                    if (result)
                    {
                        NotifyUser("Text displayed", NotifyType::StatusMessage);
                    }
                    else
                    {
                        NotifyUser("Unable to display the text", NotifyType::ErrorMessage);
                    }
                });
            });
        }
        else
        {
            NotifyUser("Unable to connect to the selected Line Display", NotifyType::ErrorMessage);
        }

    });
}
#endif
