// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_oAuthFacebook.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_oAuthFacebook::Scenario2_oAuthFacebook()
{
    InitializeComponent();

    // Use these SIDs to register the app with Facebook.
    WindowsStoreSidTextBlock->Text = WebAuthenticationBroker::GetCurrentApplicationCallbackUri()->Host;
    WindowsPhoneStoreSidTextBlock->Text = "f48e0417-873f-476c-97a2-2d25c5fa2dc7"; // copied from Package.appxmanifest
}

void Scenario2_oAuthFacebook::Launch_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FacebookReturnedToken->Text = "";

    String^ facebookURL = "https://www.facebook.com/dialog/oauth?client_id=";

    auto clientID = FacebookClientID->Text;
    if (clientID->IsEmpty())
    {
        MainPage::Current->NotifyUser("Enter a ClientID", NotifyType::ErrorMessage);
        return;
    }
    if(FacebookCallbackUrl->Text->IsEmpty())
    {
        MainPage::Current->NotifyUser("Enter a Callback URL", NotifyType::ErrorMessage);
        return;
    }

    facebookURL += clientID + "&redirect_uri=" + Uri::EscapeComponent(FacebookCallbackUrl->Text) + "&scope=read_stream&display=popup&response_type=token";

    try
    {
        auto facebookToken = FacebookReturnedToken;

        auto startURI = ref new Uri(facebookURL);
        auto endURI = ref new Uri(FacebookCallbackUrl->Text);
        MainPage::Current->NotifyUser("Navigating to: " + facebookURL + "\n", NotifyType::StatusMessage);

        create_task(WebAuthenticationBroker::AuthenticateAsync(WebAuthenticationOptions::None, startURI, endURI)).then([this](WebAuthenticationResult^ result)
        {
            String^ statusString;
            switch (result->ResponseStatus)
            {
            case WebAuthenticationStatus::ErrorHttp:
                statusString = "ErrorHttp: " + result->ResponseErrorDetail;
                break;
            case WebAuthenticationStatus::Success:
                statusString = "Success";
                FacebookReturnedToken->Text += result->ResponseData;
                break;
            case WebAuthenticationStatus::UserCancel:
                statusString = "UserCancel";
                break;
            }

            MainPage::Current->NotifyUser("Status returned by WebAuth broker: " + statusString, NotifyType::StatusMessage);

        });
    }
    catch (Exception^ ex)
    {
        MainPage::Current->NotifyUser("Error launching WebAuth " + ex->Message, NotifyType::ErrorMessage);
        return;
    }
}