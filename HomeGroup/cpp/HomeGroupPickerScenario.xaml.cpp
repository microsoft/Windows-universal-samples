// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "HomeGroupPickerScenario.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

HomeGroupPicker::HomeGroupPicker()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void HomeGroupPicker::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

// Suggest that the picker opens in homegroup.  Show how the UNC path is returned.
void HomeGroupPicker::Default_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ button = safe_cast<Button^>(sender);
    if (button != nullptr)
    {
        auto picker = ref new FileOpenPicker();
        picker->ViewMode = PickerViewMode::Thumbnail;
        picker->SuggestedStartLocation = PickerLocationId::HomeGroup;
        picker->FileTypeFilter->Append("*");
        create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
        {
            if (file)
            {
                // Application now has read/write access to the picked file
                rootPage->NotifyUser("1 file selected: " + file->Path, NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("File was not returned", NotifyType::ErrorMessage);
            }
        });
    }
}
