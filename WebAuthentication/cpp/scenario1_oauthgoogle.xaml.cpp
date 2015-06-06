// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_oAuthGoogle.xaml.h"

using namespace WebAuthentication;
using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_oAuthGoogle::Scenario1_oAuthGoogle()
{
    InitializeComponent();
}

void WebAuthentication::Scenario1_oAuthGoogle::Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ googleURL = "https://accounts.google.com/o/oauth2/auth?";
	auto clientID = GoogleClientID->Text;
	if (clientID == nullptr || clientID->IsEmpty())
	{
		MainPage::Current->NotifyUser("Enter a ClientID for Google", NotifyType::ErrorMessage);
		return;
	}
	if (GoogleCallbackUrl->Text == "") 
	{
		MainPage::Current->NotifyUser("Enter a Callback URL", NotifyType::ErrorMessage);
		return;
	}
	
	googleURL += "client_id=" + clientID + "&redirect_uri=" + Uri::EscapeComponent(GoogleCallbackUrl->Text) + "&response_type=code&scope=http%3A%2F%2Fpicasaweb.google.com%2Fdata";

	try
	{
		auto startURI = ref new Uri(googleURL);
		auto endURI = ref new Uri("https://accounts.google.com/o/oauth2/approval?");
		MainPage::Current->NotifyUser("Navigating to: " + googleURL + "\n", NotifyType::StatusMessage);

		create_task(WebAuthenticationBroker::AuthenticateAsync(WebAuthenticationOptions::UseTitle, startURI, endURI)).then([this](WebAuthenticationResult^ result)
		{
			String^ statusString;
			switch (result->ResponseStatus)
			{
			case WebAuthenticationStatus::ErrorHttp:
				statusString = "ErrorHttp: " + result->ResponseErrorDetail;
				break;
			case WebAuthenticationStatus::Success:
				statusString = "Success";
				GoogleReturnedToken->Text += result->ResponseData;
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