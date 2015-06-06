// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_AddReadRemoveCredentials.xaml.h"

using namespace PasswordVault;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Credentials;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_AddReadRemoveCredentials::Scenario1_AddReadRemoveCredentials()
{
    InitializeComponent();
	// Initialize the password vault, this may take less than a second
	// An optimistic initialization at this stage improves the UI performance
	// when any other call to the password vault may be made later on
	auto task = InitializeVaultAsync();
}

void Scenario1_AddReadRemoveCredentials::Save_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try {
		auto resource = InputResourceValue->Text;
		auto username = InputUserNameValue->Text;
		auto password = InputPasswordValue->Password;

		if (!resource || !username || !password)
		{
			MainPage::Current->NotifyUser("Resource, User name and password are not allowed to be empty, Please input resource, user name and password", NotifyType::ErrorMessage);
			return;
		}

		else
		{
			//
			//Adding a credential to PasswordVault.
			//
			Windows::Security::Credentials::PasswordVault^ vault = ref new Windows::Security::Credentials::PasswordVault();
			PasswordCredential^ c = ref new PasswordCredential(resource, username, password);
			vault->Add(c);
			MainPage::Current->NotifyUser("Credential saved successfully. Resource: " + resource + " Username: " + username + " Password:" + password + ".", NotifyType::StatusMessage);
			return;
		}
	}
	catch (Platform::COMException^ e)
	{
		MainPage::Current->NotifyUser(e->Message, NotifyType::ErrorMessage);
		return;
	}
}

void Scenario1_AddReadRemoveCredentials::Read_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try {
		auto resource = InputResourceValue->Text;
		auto username = InputUserNameValue->Text;
		auto password = InputPasswordValue->Password;
		Windows::Foundation::Collections::IVectorView<PasswordCredential^>^ creds;
		PasswordCredential^ cred;

		//
		//Reading a credential from PasswordVault.
		//
		Windows::Security::Credentials::PasswordVault^ vault = ref new Windows::Security::Credentials::PasswordVault();

		//
		//If both resource and username are empty, you can use RetrieveAll() to enumerate all credentials
		//
		if (!resource&&!username)
		{
			MainPage::Current->NotifyUser("Retrieving all credentials since resource or username are not specified.", NotifyType::StatusMessage);
			creds = vault->RetrieveAll();
		}
		//
		//If there is only resouce, you can use FindAllByResource() to enumerate by resource.
		//
		else if (!username)
		{
			MainPage::Current->NotifyUser("Retrieving credentials in PasswordVault by resource: " + resource, NotifyType::StatusMessage);
			creds = vault->FindAllByResource(resource);
		}
		//
		//If there is only username, you can use findbyusername() to enumerate by resource.
		//
		else if (!resource)
		{
			MainPage::Current->NotifyUser("Retrieving credentials in PasswordVault by username: " + username, NotifyType::StatusMessage);
			creds = vault->FindAllByUserName(username);
		}
		//
		//If both resource and username are provided, you can use Retrieve to search the credential
		//
		else
		{
			MainPage::Current->NotifyUser("Retrieving credentials in PasswordVault by resource and username: " + resource + "/" + username, NotifyType::StatusMessage);
			cred = vault->Retrieve(resource, username);
		}

		//
		//print out search result in debug output
		//
		if (cred)
		{
			MainPage::Current->NotifyUser("Read credential successfully. Resource: " + cred->Resource + ", Username: " + cred->UserName + " Password: " + cred->Password + ".", NotifyType::StatusMessage);

		}
		else if (creds->Size > 0)
		{
			MainPage::Current->NotifyUser("There are(is) " + creds->Size + " credential(s) found in PasswordVault", NotifyType::StatusMessage);

			for each (PasswordCredential^ c in creds)
			{
				MainPage::Current->NotifyUser("Read credential successfully. Resource:" + c->Resource + ", Username: " + c->UserName + ".", NotifyType::StatusMessage);
			}
		}
		else
			MainPage::Current->NotifyUser("Credential not found.", NotifyType::ErrorMessage);
		return;
	}
	catch (Platform::COMException^ e)
	{
		if (e->HResult == 0x80070490)
		{
			MainPage::Current->NotifyUser("Credential not found.", NotifyType::ErrorMessage);
		}
		else
		{
			MainPage::Current->NotifyUser(e->Message, NotifyType::ErrorMessage);
		}
		return;
	}
}

void Scenario1_AddReadRemoveCredentials::Delete_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try {
		auto resource = InputResourceValue->Text;
		auto username = InputUserNameValue->Text;
		PasswordCredential^ cred;
		//
		//Deleting a credential from PasswordVault.
		//
		Windows::Security::Credentials::PasswordVault^ vault = ref new Windows::Security::Credentials::PasswordVault();
		//
		//If both resource and username are empty, you can use RetrieveAll() to enumerate all credentials
		//
		if (!resource || !username)
		{
			MainPage::Current->NotifyUser("Please provide both resource and username to delete a credential", NotifyType::ErrorMessage);
		}
		else
		{
			cred = vault->Retrieve(resource, username);
			vault->Remove(cred);
			MainPage::Current->NotifyUser("Credential removed successfully. Resource: " + resource + " Username: " + username + ".", NotifyType::StatusMessage);
		}
		return;
	}
	catch (Platform::COMException^ e)
	{
		if (e->HResult == 0x80070490)
		{
			MainPage::Current->NotifyUser("Credential not found.", NotifyType::ErrorMessage);
		}
		else
		{
			MainPage::Current->NotifyUser(e->Message, NotifyType::ErrorMessage);
		}
		return;
	}
}

void Scenario1_AddReadRemoveCredentials::Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try {
		auto resource = InputResourceValue->Text;
		auto username = InputUserNameValue->Text;
		Windows::Foundation::Collections::IVectorView<PasswordCredential^>^ creds;
		//
		//Enumerate and delete all credentials from PasswordVault.
		//
		Windows::Security::Credentials::PasswordVault^ vault = ref new Windows::Security::Credentials::PasswordVault();
		creds = vault->RetrieveAll();
		if (creds)
		{
			if (creds->Size>0)
			{
				for each (PasswordCredential^ c in creds)
				{
					vault->Remove(c);
				}
			}
			MainPage::Current->NotifyUser("Removing " + creds->Size + "credentials from PasswordVault in total", NotifyType::StatusMessage);
		}

		InputResourceValue->Text = "";
		InputUserNameValue->Text = "";
		InputPasswordValue->Password = "";

		MainPage::Current->NotifyUser("Scenario 1 is reset", NotifyType::StatusMessage);
		return;
	}
	catch (Platform::COMException^ e)
	{
		MainPage::Current->NotifyUser(e->Message, NotifyType::ErrorMessage);
		return;
	}
}

Windows::Foundation::IAsyncAction^ Scenario1_AddReadRemoveCredentials::InitializeVaultAsync()
{
	// any call to the password vault will load the vault
	return create_async([]() {
		Windows::Security::Credentials::PasswordVault^ vault = ref new Windows::Security::Credentials::PasswordVault();
		vault->RetrieveAll();
	});
}
