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
#include "Scenario5.h"
#include "Scenario5.g.cpp"
#include "SampleConfiguration.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5::Scenario5()
    {
        InitializeComponent();
    }

    void Scenario5::OnNavigatedTo(NavigationEventArgs const&)
    {
        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock(), OutputDetails());
    }

    fire_and_forget Scenario5::GetThumbnailButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        Helpers::ResetOutput(rootPage, ThumbnailImage(), OutputTextBlock(), OutputDetails());

        // Pick a folder
        FolderPicker folderPicker;
        folderPicker.FileTypeFilter().ReplaceAll(FileExtensions::Image());
        StorageFolder folder = co_await folderPicker.PickSingleFolderAsync();

        if (folder != nullptr)
        {
            const CommonFolderQuery monthShape = CommonFolderQuery::GroupByMonth;
            // Verify queries are supported because they are not supported in all picked locations.
            if (folder.IsCommonFolderQuerySupported(monthShape))
            {
                // Convert folder to file group and query for items
                IVectorView<StorageFolder> months = co_await folder.CreateFolderQuery(monthShape).GetFoldersAsync();

                if (months.Size() > 0)
                {
                    const ThumbnailMode thumbnailMode = ThumbnailMode::PicturesView;
                    const uint32_t size = 200;
                    StorageFolder firstMonth = months.GetAt(0);
                    StorageItemThumbnail thumbnail = co_await firstMonth.GetThumbnailAsync(thumbnailMode, size);
                    if (thumbnail != nullptr)
                    {
                        Helpers::DisplayResult(ThumbnailImage(), OutputTextBlock(), thumbnailMode, size, firstMonth, thumbnail, true);

                        // Also display the hierarchy of the file group to better visualize where the thumbnail comes from
                        IVectorView<StorageFile> files = co_await firstMonth.GetFilesAsync();

                        std::wstringstream output;
                        output << std::endl;
                        output << L"List of files in this group:" << std::endl << std::endl;
                        for (StorageFile const& file : files)
                        {
                            output << std::wstring_view{ file.Name() } << std::endl;
                        }
                        OutputDetails().Text(output.str());
                        thumbnail.Close();
                    }
                    else
                    {
                        rootPage.NotifyUser(Errors::NoImages, NotifyType::StatusMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser(Errors::FileGroupEmpty, NotifyType::StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser(Errors::FileGroupLocation, NotifyType::StatusMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    }
}
