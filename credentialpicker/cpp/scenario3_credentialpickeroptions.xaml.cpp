// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario3_CredentialPickerOptions.xaml.h"

using namespace CredentialPicker;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Credentials::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_CredentialPickerOptions::Scenario3_CredentialPickerOptions()
{
    InitializeComponent();
}

void Scenario3_CredentialPickerOptions::SetResult(CredentialPickerResults^ result)
{
	auto domainName = result->CredentialDomainName;
	auto userName = result->CredentialUserName;
	auto password = result->CredentialPassword;
	auto savedByApi = result->CredentialSaved;
	auto saveOption = result->CredentialSaveOption;

	Status->Text = "OK";
	Domain->Text = domainName;
	Username->Text = userName;
	Password->Text = password;
	CredentialSaved->Text = (savedByApi ? "true" : "false");
	switch (result->CredentialSaveOption)
	{
	case Windows::Security::Credentials::UI::CredentialSaveOption::Hidden:
		CheckboxState->Text = "Hidden";
		break;
	case Windows::Security::Credentials::UI::CredentialSaveOption::Selected:
		CheckboxState->Text = "Selected";
		break;
	case Windows::Security::Credentials::UI::CredentialSaveOption::Unselected:
		CheckboxState->Text = "Unselected";
		break;
	}

}

void Scenario3_CredentialPickerOptions::SetPasswordExplainVisibility(bool isShown)
{
	if (isShown)
	{
		PasswordExplain1->Visibility = Windows::UI::Xaml::Visibility::Visible;
		PasswordExplain2->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else
	{
		PasswordExplain1->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		PasswordExplain2->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
}

void Scenario3_CredentialPickerOptions::Launch_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if ((Message->Text == nullptr) || (Caption->Text == nullptr) || (Target->Text == nullptr))
	{
		return;
	}

	CredentialPickerOptions^ credPickerOptions = ref new CredentialPickerOptions();
	credPickerOptions->AlwaysDisplayDialog = AlwaysShowDialog->IsChecked->Value;
	credPickerOptions->Message = Message->Text;
	credPickerOptions->Caption = Caption->Text;
	credPickerOptions->TargetName = Target->Text;
	if (ProtocolSelection->SelectedItem == nullptr)
	{
		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Negotiate;
	}
	else
	{
		String^ protocolName = ((ComboBoxItem^)ProtocolSelection->SelectedItem)->Content->ToString();
		if (protocolName->Equals("Negotiate"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Negotiate;
		}
		else if (protocolName->Equals("NTLM"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Ntlm;
		}
		else if (protocolName->Equals("Kerberos"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Kerberos;
		}
		else if (protocolName->Equals("CredSsp"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::CredSsp;
		}
		else if (protocolName->Equals("Basic"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Basic;
		}
		else if (protocolName->Equals("Digest"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Digest;
		}
		else if (protocolName->Equals("Custom"))
		{
			credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Custom;
			credPickerOptions->CustomAuthenticationProtocol = CustomProtocol->Text;
		}
		else
		{
			MainPage::Current->NotifyUser("Unknown Protocol", NotifyType::ErrorMessage);
		}
	}
	if (SaveCheckboxSelection->SelectedItem != nullptr)
	{
		String^ checkboxState = ((ComboBoxItem^)SaveCheckboxSelection->SelectedItem)->Content->ToString();
		if (checkboxState->Equals("Hidden"))
		{
			credPickerOptions->CredentialSaveOption = Windows::Security::Credentials::UI::CredentialSaveOption::Hidden;
		}
		else if (checkboxState->Equals("Selected"))
		{
			credPickerOptions->CredentialSaveOption = Windows::Security::Credentials::UI::CredentialSaveOption::Selected;
		}
		else if (checkboxState->Equals("Unselected"))
		{
			credPickerOptions->CredentialSaveOption = Windows::Security::Credentials::UI::CredentialSaveOption::Unselected;
		}
		else
		{
			MainPage::Current->NotifyUser("Unknown Checkbox state", NotifyType::ErrorMessage);
		}
	}
	create_task(Windows::Security::Credentials::UI::CredentialPicker::PickAsync(credPickerOptions))
		.then([this](CredentialPickerResults^ credPickerResult)
	{
		SetResult(credPickerResult);
	});


}

void Scenario3_CredentialPickerOptions::Protocol_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	ComboBox^ box = (ComboBox^)sender;
	if (ProtocolSelection == nullptr || ProtocolSelection->SelectedItem == nullptr || CustomProtocol == nullptr)
	{
		//return if this was triggered before all components are initialized
		return;
	}

	String^ selectedProtocol = ProtocolSelection->SelectedItem->ToString();

	if (selectedProtocol->Equals("Custom"))
	{
		CustomProtcolStackPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else
	{
		CustomProtcolStackPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		//Basic and Digest return plaintext passwords
		if (selectedProtocol->Equals("Basic"))
		{
			SetPasswordExplainVisibility(false);
		}
		else if (selectedProtocol->Equals("Digest"))
		{
			SetPasswordExplainVisibility(false);
		}
		else
		{
			SetPasswordExplainVisibility(true);
		}
	}
}
