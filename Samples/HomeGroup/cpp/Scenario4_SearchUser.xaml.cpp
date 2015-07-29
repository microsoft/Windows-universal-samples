// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4_SearchUser.xaml.h"

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

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace concurrency;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace std;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;


Scenario4_SearchUser::Scenario4_SearchUser()
{
    InitializeComponent();
}

/// <summary>
/// Scenario 4 initialization.  Create a button for each HomeGroup user.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4_SearchUser::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    // This try/catch block is for scenarios where the HomeGroup Known Folder is not available.
    try
    {
        // Enumerate the HomeGroup first-level folders, which represent the HomeGroup users.
        create_task(KnownFolders::HomeGroup->GetFoldersAsync()).then(
            [this](IVectorView<StorageFolder^>^ folders)
        {
            if ((folders != nullptr) && (folders->Size != 0))
            {
                // Create a button for each user in the HomeGroup.
                // The DataContext is the folder that represents the user's files.
                for (StorageFolder^ folder : folders)
                {
                    Button^ button = ref new Button();
                    button->DataContext = folder;
                    button->Style = safe_cast<Windows::UI::Xaml::Style^>(Resources->Lookup("UserButtonStyle"));
                    button->Content = folder->Name;
                    button->Click += ref new RoutedEventHandler(this, &Scenario4_SearchUser::UserButton_Click);
                    UserGrid->Items->Append(button);
                }
            }
            else
            {
                rootPage->NotifyUser("No HomeGroup users found. Make sure you are joined to a HomeGroup and there is someone sharing.", NotifyType::ErrorMessage);
            }
        });
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser(ex->Message, NotifyType::StatusMessage);
    }
}


void Scenario4_SearchUser::UserButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    Button^ button = safe_cast<Button^>(sender);
    if (button != nullptr)
    {
        OutputProgressRing->IsActive = true;

        // Get the folder that represents the user's files, which wwe saved as the DataContext.
        StorageFolder^ folder = safe_cast<StorageFolder^>(button->DataContext);

        // This try/catch block is for scenarios where the HomeGroup Known Folder is not available.
        try
        {
            // Search for all files in that folder.
            auto queryOptions = ref new QueryOptions(CommonFileQuery::OrderBySearchRank, nullptr);
            queryOptions->UserSearchFilter = "*";
            auto queryResult = folder->CreateFileQueryWithOptions(queryOptions);
            create_task(queryResult->GetFilesAsync()).then(
                [this, folder](IVectorView<StorageFile^>^ files)
                {
                    String^ outputString = "Files shared by " + folder->Name + ": " + files->Size.ToString() + "\n";
                    for (StorageFile^ file : files)
                    {
                        outputString += file->Name + "\n";
                    };
                    rootPage->NotifyUser(outputString, NotifyType::StatusMessage);
                });
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser(ex->Message, NotifyType::StatusMessage);
        }
    }
}
