// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_Search.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_Search::Scenario2_Search()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_Search::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario2_Search::Search_Click(Object^ sender, RoutedEventArgs^ e)
{
    Button^ button = safe_cast<Button^>(sender);
    if (button != nullptr)
    {
        rootPage->NotifyUser("", NotifyType::StatusMessage);
        OutputProgressRing->IsActive = true;
        auto queryOptions = ref new QueryOptions(CommonFileQuery::OrderBySearchRank, nullptr);
        queryOptions->UserSearchFilter = QueryInputBox->Text;

        // This try/catch block is for scenarios where the HomeGroup Known Folder is not available.
        try
        {
            auto queryResult = KnownFolders::HomeGroup->CreateFileQueryWithOptions(queryOptions);
            create_task(queryResult->GetFilesAsync()).then([this, queryOptions](IVectorView<StorageFile^>^ files)
            {
                String^ outputString = "Files found: " + files->Size.ToString() + "\n";
                for (StorageFile^ file : files)
                {
                    outputString += file->Name + "\n";
                }
                rootPage->NotifyUser(outputString, NotifyType::StatusMessage);
                OutputProgressRing->IsActive = false;
            });
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser(ex->Message, NotifyType::StatusMessage);
        }
    }
}
