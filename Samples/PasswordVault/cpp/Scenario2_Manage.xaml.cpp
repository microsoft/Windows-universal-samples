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
#include "Scenario2_Manage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Credentials;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_Manage::Scenario2_Manage()
{
    InitializeComponent();
}

void Scenario2_Manage::RetrieveCredentials()
{
    auto resource = InputResourceValue->Text;
    auto userName = InputUserNameValue->Text;

    // Clear previous output.
    RetrievedCredentials->Items->Clear();
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    auto vault = ref new PasswordVault();
    IVectorView<PasswordCredential^>^ creds = nullptr;

    // The credential retrieval functions raise an "Element not found"
    // exception if there were no matches.
    try
    {
        // Perform the desired queryInputUserNameValue.Text
        if (!resource && !userName)
        {
            // Both resource and user name are empty: Use retrieveAll().
            creds = vault->RetrieveAll();
        }
        else if (!userName)
        {
            // Resource is provided but no user name: Use findAllByResource().
            creds = vault->FindAllByResource(resource);
        }
        else if (!resource)
        {
            // User name is provided but no resource: Use findByUserName().
            creds = vault->FindAllByUserName(userName);
        }
        else
        {
            // Both resource and user name are provided: Use retrieve().
            PasswordCredential^ cred = vault->Retrieve(resource, userName);
            // Add it to our results if the retrieval was successful.
            if (cred != nullptr)
            {
                auto vector = ref new Vector<PasswordCredential^>();
                vector->Append(cred);
                creds = vector->GetView();
            }
        }
    }
    catch (Exception^ e)
    {
        if (e->HResult != ElementNotFound)
        {
            throw;
        }
    }

    // Display the results. Note that the password field is initially blank.
    // You must call retrievePassword() to get the password.

    if (creds == nullptr || creds->Size == 0)
    {
        rootPage->NotifyUser("No matching credentials", NotifyType::ErrorMessage);
    }
    else
    {
        ItemCollection^ items = RetrievedCredentials->Items;
        for (PasswordCredential^ cred : creds)
        {
            auto item = ref new TextBlock();
            item->DataContext = cred;
            item->Text = cred->Resource + ": " + cred->UserName;
            items->Append(item);
        }
        rootPage->NotifyUser("Credentials found: " + items->Size.ToString(), NotifyType::StatusMessage);
    }
}

void Scenario2_Manage::RevealPasswords()
{
    IVector<Object^>^ selectedItems = RetrievedCredentials->SelectedItems;
    if (selectedItems->Size > 0)
    {
        int retrievedCount = 0;
        for (Object^ o : selectedItems)
        {
            TextBlock^ item = safe_cast<TextBlock^>(o);
            PasswordCredential^ cred = safe_cast<PasswordCredential^>(item->DataContext);
            try
            {
                cred->RetrievePassword();
                item->Text = cred->Resource + ": " + cred->UserName + " (" + cred->Password + ")";
                retrievedCount++;
            }
            catch (Exception^ e)
            {
                if (e->HResult != ElementNotFound)
                {
                    throw;
                }
                item->Text = cred->Resource + ": " + cred->UserName + " (password unavailable)";
            }
        }
        rootPage->NotifyUser("Passwords retrieved: " + retrievedCount.ToString(), NotifyType::StatusMessage);
    }
    else
    {
        rootPage->NotifyUser("No credentials selected", NotifyType::ErrorMessage);
    }
}

void Scenario2_Manage::RemoveCredentials()
{
    IVector<Object^>^ selectedItems = RetrievedCredentials->SelectedItems;
    if (selectedItems->Size > 0)
    {
        auto vault = ref new PasswordVault();
        int removedCount = 0;

        // Take a snapshot of the selectedOptions collection because we will be
        // changing the collection during the loop.
        auto snapshot = ref new Vector<Object^>(begin(selectedItems), end(selectedItems));

        for (Object^ o : snapshot)
        {
            TextBlock^ item = safe_cast<TextBlock^>(o);
            PasswordCredential^ cred = safe_cast<PasswordCredential^>(item->DataContext);
            unsigned int index;
            if (RetrievedCredentials->Items->IndexOf(o, &index))
            {
                RetrievedCredentials->Items->RemoveAt(index);
            }

            try
            {
                vault->Remove(cred);
                removedCount++;
            }
            catch (Exception^ e)
            {
                // Ignore exception if the credential was already removed.
                if (e->HResult != ElementNotFound)
                {
                    throw;
                }
            }
        }
        rootPage->NotifyUser("Credentials removed: " + removedCount.ToString(), NotifyType::StatusMessage);
    }
    else
    {
        rootPage->NotifyUser("No credentials selected", NotifyType::ErrorMessage);
    }
}
