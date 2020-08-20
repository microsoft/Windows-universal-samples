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
#include "Scenario2_Launch.h"
#include "Scenario2_Launch.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Launch::Scenario2_Launch()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_Launch::CheckIf3DBuilderIsInstalled_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        IVectorView<AppInfo> handlers = co_await Launcher::FindFileHandlersAsync(L".3mf");
        auto found = std::find_if(begin(handlers), end(handlers),
            [](auto&& info) { return info.PackageFamilyName() == PackageFamilyName3DBuilder; });
        if (found != end(handlers))
        {
            rootPage.NotifyUser(L"3D Builder is installed", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"3D Builder is not installed", NotifyType::ErrorMessage);
        }
    }

    fire_and_forget Scenario2_Launch::LaunchFileIn3DBuilder_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        FileOpenPicker openPicker;
        openPicker.ViewMode (PickerViewMode::Thumbnail);
        openPicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        openPicker.FileTypeFilter().ReplaceAll({ L".3mf", L".stl" });

        StorageFile file = co_await openPicker.PickSingleFileAsync();
        if (file == nullptr)
        {
            co_return;
        }

        LauncherOptions options;

        // The target application is 3D Builder.
        options.TargetApplicationPackageFamilyName(PackageFamilyName3DBuilder);

        // If 3D Builder is not installed, redirect the user to the Store to install it.
        options.FallbackUri(Uri(hstring(L"ms-windows-store:PDP?PFN=") + PackageFamilyName3DBuilder));

        co_await Launcher::LaunchFileAsync(file, options);
    }
}
