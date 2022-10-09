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
#include "Scenario2_Manage.h"
#include "Scenario2_Manage.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Security::Credentials;

constexpr auto hresult_not_found = winrt::hresult{ static_cast<int32_t>(0x80070490) };

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Manage::Scenario2_Manage()
    {
        InitializeComponent();
    }

    void Scenario2_Manage::RetrieveCredentials_Click(IInspectable const&, IInspectable const&)
    {
        auto resource = InputResourceValue().Text();
        auto userName = InputUserNameValue().Text();

        // Clear previous output.
        RetrievedCredentials().Items().Clear();
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        PasswordVault vault;
        IVectorView<PasswordCredential> creds{ nullptr };

        // The credential retrieval functions raise an "Element not found"
        // exception if there were no matches.
        try
        {
            // Perform the desired queryuserName
            if (resource.empty() && userName.empty())
            {
                // Both resource and user name are empty: Use retrieveAll().
                creds = vault.RetrieveAll();
            }
            else if (userName.empty())
            {
                // Resource is provided but no user name: Use findAllByResource().
                creds = vault.FindAllByResource(resource);
            }
            else if (resource.empty())
            {
                // User name is provided but no resource: Use findByUserName().
                creds = vault.FindAllByUserName(userName);
            }
            else
            {
                // Both resource and user name are provided: Use retrieve().
                // Add it to our results if the retrieval was successful.
                if (PasswordCredential cred = vault.Retrieve(resource, userName))
                {
                    creds = winrt::single_threaded_vector<PasswordCredential>({ cred }).GetView();
                }
            }
        }
        catch (winrt::hresult_error const& e)
        {
            if (e.code() != hresult_not_found)
            {
                throw;
            }
        }

        // Display the results. Note that the password field is initially blank.
        // You must call retrievePassword() to get the password.

        if (!creds || creds.Size() == 0)
        {
            rootPage.NotifyUser(L"No matching credentials", NotifyType::ErrorMessage);
        }
        else
        {
            ItemCollection items = RetrievedCredentials().Items();
            for (PasswordCredential const& cred : creds)
            {
                TextBlock item;
                item.DataContext(cred);
                item.Text(cred.Resource() + L": " + cred.UserName());
                items.Append(item);
            }
            rootPage.NotifyUser(L"Credentials found: " + std::to_wstring(items.Size()), NotifyType::StatusMessage);
        }
    }

    void Scenario2_Manage::RevealPasswords_Click(IInspectable const&, IInspectable const&)
    {
        auto selectedItems = RetrievedCredentials().SelectedItems();
        if (selectedItems.Size() > 0)
        {
            int retrievedCount = 0;
            for(IInspectable const& o : selectedItems)
            {
                auto item = o.as<TextBlock>();
                auto cred = item.DataContext().as<PasswordCredential>();

                // The credential retrieval functions raise an "Element not found"
                // exception if the credential is no longer in the PasswordVault.
                // (For example, if the user manually removed the credential via the
                // Control Panel.)
                try
                {
                    cred.RetrievePassword();
                    item.Text(cred.Resource() + L": " + cred.UserName() + L" (" + cred.Password() + L")");
                    retrievedCount++;
                }
                catch (winrt::hresult_error const& e)
                {
                    if (e.code() != hresult_not_found)
                    {
                        throw;
                    }

                    item.Text(cred.Resource() + L": " + cred.UserName() + L" (password unavailable)");
                }

            }

            rootPage.NotifyUser(L"Passwords retrieved: " + std::to_wstring(retrievedCount), NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"No credentials selected", NotifyType::ErrorMessage);
        }
    }

    void Scenario2_Manage::RemoveCredentials_Click(IInspectable const&, IInspectable const&)
    {
        auto selectedItems = RetrievedCredentials().SelectedItems();
        if (selectedItems.Size() > 0)
        {
            PasswordVault vault;
            int removedCount = 0;

            // Take a snapshot of the selectedOptions collection because we will be
            // changing the collection during the loop.
            std::vector<IInspectable> snapshot{ selectedItems.begin(), selectedItems.end() };

            for (auto &&o : snapshot)
            {
                TextBlock item = o.as<TextBlock>();
                PasswordCredential cred = item.DataContext().as<PasswordCredential>();
                uint32_t index;
                if (RetrievedCredentials().Items().IndexOf(o, index))
                {
                    RetrievedCredentials().Items().RemoveAt(index);
                }

                try
                {
                    vault.Remove(cred);
                    removedCount++;
                }
                catch (hresult_error const& e)
                {
                    // Ignore exception if the credential was already removed.
                    if (e.code() != hresult_not_found)
                    {
                        throw;
                    }
                }
            }

            rootPage.NotifyUser(L"Credentials removed: " + std::to_wstring(removedCount), NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"No credentials selected", NotifyType::ErrorMessage);
        }
    }

}

