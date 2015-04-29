// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SharePage.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Xaml; 
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Share text",                 "SDKTemplate.ShareText" },
    { "Share web link",             "SDKTemplate.ShareWebLink" },
    { "Share application link",     "SDKTemplate.ShareApplicationLink" },
    { "Share image",                "SDKTemplate.ShareImage" },
    { "Share files",                "SDKTemplate.ShareFiles" },
    { "Share delay rendered files", "SDKTemplate.ShareDelayRenderedFiles" },
    { "Share HTML content",         "SDKTemplate.ShareHtml" },
    { "Share custom data",          "SDKTemplate.ShareCustomData" },
    { "Fail with display text",     "SDKTemplate.SetErrorMessage" }
};

void App::OnActivated(IActivatedEventArgs^ args)
{
    Frame^ rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
    if (rootFrame == nullptr)
    {
        rootFrame = ref new Frame();
        rootFrame->Language = Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);
        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);
        rootFrame->Navigate(TypeName(MainPage::typeid));
        Window::Current->Content = rootFrame;
    }

    // Protocol activation is the only type of activation that this app handles
    if (args->Kind == ActivationKind::Protocol)
    {
        ProtocolActivatedEventArgs^ protocolArgs = (ProtocolActivatedEventArgs^)args;
        ((MainPage^)rootFrame->Content)->LoadScenarioForProtocolActivation(protocolArgs);
    }

    Window::Current->Activate();
}

void MainPage::LoadScenarioForProtocolActivation(ProtocolActivatedEventArgs^ protocolArgs)
{
    for (size_t i = 0; i < scenarios->Length; i++)
    {
        if (protocolArgs->Uri->Equals(SharePage::GetApplicationLink(scenarios[i].ClassName)))
        {
            // By setting the selected index, the Scenarios_SelectionChanged(...) method will be called
            ScenarioControl->SelectedIndex = i;
            break;
        }
    }
}
