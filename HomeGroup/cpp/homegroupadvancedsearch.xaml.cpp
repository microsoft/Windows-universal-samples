// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "HomeGroupAdvancedSearch.xaml.h"

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


HomeGroupAdvancedSearch::HomeGroupAdvancedSearch()
{
    InitializeComponent();
}

/// <summary>
/// Scenario 4 initialization.  Find the homegroup users and their thumbnails and draw them as input options.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void HomeGroupAdvancedSearch::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
    SetButtons();
}

void HomeGroupAdvancedSearch::SetButtons()
{
    // This try/catch block is for scenarios where the Homegroup Known Folder is not available.
    try
    {
        auto homeGroupFolder = KnownFolders::HomeGroup;
        // Enumerate the homegroup first-level folders, which represent the homegroup users, and get the thumbnails.
        create_task(homeGroupFolder->GetFoldersAsync()).then(
            [this, homeGroupFolder](IVectorView<StorageFolder^>^ folders)
        {
            if (folders)
            {
                // HomeGroup can have more than 4 users. Iterating and limiting it to '4'
                // for this sample.
                int userCount = ((folders->Size < 4) ? folders->Size : 4);
                Vector<Windows::UI::Xaml::Controls::Button^>^ buttons = ref new Vector<Windows::UI::Xaml::Controls::Button^>;
                buttons->InsertAt(0, user1Button);
                buttons->InsertAt(1, user2Button);
                buttons->InsertAt(2, user3Button);
                buttons->InsertAt(3, user4Button);
                for (int userIndex = 0; userIndex < userCount; userIndex++)
                {
                    buttons->GetAt(userIndex)->Content = folders->GetAt(userIndex)->Name;
                    buttons->GetAt(userIndex)->Visibility = Windows::UI::Xaml::Visibility::Visible;
                }
            }
            else
            {
                rootPage->NotifyUser("No homegroup users could be found.  Make sure you are joined to a homegroup and there is someone sharing", NotifyType::ErrorMessage);
            }
        });
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser(ex->Message, NotifyType::StatusMessage);
    }
}


void HomeGroupAdvancedSearch::Default_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ button = safe_cast<Button^>(sender);
    if (button != nullptr)
    {
        // This try/catch block is for scenarios where the Homegroup Known Folder is not available.
        try
        {
            StorageFolder^ homeGroupFolder = KnownFolders::HomeGroup;
            Platform::String^ userName = button->Content->ToString();
            create_task(homeGroupFolder->GetFoldersAsync()).then(
                [this, userName](IVectorView<StorageFolder^>^ folders)
            {
                std::for_each(begin(folders), end(folders), [this, userName](StorageFolder ^folder)
                {
                    if (folder->Name->Equals(userName))
                    {
                        auto fileTypeFilter = ref new Vector<Platform::String^>();
                        fileTypeFilter->Append("*");
                        auto queryOptions = ref new QueryOptions(CommonFileQuery::OrderBySearchRank, fileTypeFilter);
                        auto queryResult = folder->CreateFileQueryWithOptions(queryOptions);
                        create_task(queryResult->GetFilesAsync()).then(
                            [this, queryOptions, userName](IVectorView<StorageFile^>^ files)
                        {
                            if (files->Size == 0)
                            {
                                rootPage->NotifyUser("No files found for " + userName, NotifyType::ErrorMessage);
                            }
                            else
                            {
                                Platform::String^ outputText = files->Size.ToString();
                                outputText += (files->Size == 1) ? " file shared by " + userName + "\n\n"
                                    : " files shared by " + userName + "\n\n";
                                std::for_each(begin(files), end(files), [this, &outputText](StorageFile ^file)
                                {
                                    outputText += file->Name + "\n";
                                });
                                rootPage->NotifyUser(outputText, NotifyType::StatusMessage);
                            }
                        });
                    }
                });
            });
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser(ex->Message + "\n", NotifyType::StatusMessage);
        }
    }
}
