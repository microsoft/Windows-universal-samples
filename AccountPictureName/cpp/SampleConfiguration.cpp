// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Get display name", "SDKTemplate.GetUserDisplayName" },
    { "Get first and last name", "SDKTemplate.GetUserFirstAndLastName" },
    { "Get account picture", "SDKTemplate.GetAccountPicture" },
    { "Set account picture and listen for changes", "SDKTemplate.SetAccountPicture" }
};

void App::OnActivated(IActivatedEventArgs^ args)
{
    // Check to see if the app was activated via a protocol
    if (args->Kind == ActivationKind::Protocol)
    {
        auto protocolArgs = safe_cast<ProtocolActivatedEventArgs^>(args);
        if (protocolArgs->Uri->SchemeName->Equals("ms-accountpictureprovider"))
        {
            if (Window::Current->Content == nullptr)
            {
                auto rootFrame = ref new Frame();
                TypeName pageType = { "SDKTemplate.MainPage", TypeKind::Custom };
                rootFrame->Navigate(pageType);
                Window::Current->Content = rootFrame;
                Window::Current->Activate();
            }

            MainPage::Current->NavigateToScenario("SDKTemplate.SetAccountPicture");
        }
    }
}
