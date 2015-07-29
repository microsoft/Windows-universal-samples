//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario13.xaml.cpp
// Implementation of the Scenario13 class
//

#include "pch.h"
#include "Scenario13.xaml.h"

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
using namespace Windows::ApplicationModel::Resources::Core;
using namespace concurrency;
using namespace Windows::Storage;


Scenario13::Scenario13()
{
    InitializeComponent();
    DXFLOptionCombo->SelectedIndex = 0;
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario13::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void Scenario13::DXFLOptionCombo_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    ComboBox^ combo = safe_cast<ComboBox^>(sender);

    // Setting the DXFeatureLevel qualifier value on all default contexts. This is needed
    // since the async operation to retrive appdata.dat as a StorageFile object will happen 
    // on a background thread. This is the only way to set the qualifier on a default context 
    // used in a background thread.

    // In a different scenario, rather than obtaining a StorageFile object, another approach
    // that could be used would be to use the ResourceMap.GetValue method to get the resolved
    // resource candidate, and from that get the absolute file path. In that approach, the
    // GetValue method takes a ResourceContext parameter, and so it would be possible to set
    // the DXFeatureLevel qualifier value on a specific context rather than setting it in all
    // contexts within the app.

    ResourceContext::SetGlobalQualifierValue("dxfeaturelevel", combo->SelectedValue->ToString());


    // setting up the resource URI -- an ms-appx URI will be used to access the resource using
    // a StorageFile method

    // In a different scenario, rather than obtaining a StorageFile object, the 
    // ResourceMap.GetValue method could be used to get the resolved candidate, from which an 
    // absolute file path could be obtained. However, resource references used by the APIs in 
    // Windows.ApplicationModel.Resources and Windows.ApplicationModel.Resources.Core use 
    // ms-resource URIs, not ms-appx URIs. Within a resource map, files are always organized 
    // under a "Files" top-level subtree. The ms-resource URI string for the file being accessed 
    // here would be "ms-resource:///Files/appdata/appdata.dat".

    // The URI string "ms-appx:///..." is rooted at the app package root.
    Uri^ uri = ref new Uri("ms-appx:///appdata/appdata.dat");

    // need a variable to capture in the lambda for the async continuation
    TextBlock^ resultTextControl = this->ResultText;

    // The resource candidate will be resolved during the call to GetFileFromApplicationUriAsync 
    // using the default context from a background thread.

    // File IO uses asynchronous techniques. For more info, see
    // http://msdn.microsoft.com/en-us/library/windows/apps/hh464924.aspx

    auto getFileTask = create_task(StorageFile::GetFileFromApplicationUriAsync(uri));
    getFileTask.then([resultTextControl](StorageFile^ file) {
        return FileIO::ReadTextAsync(file);
    }).then([resultTextControl](Platform::String^ content) {
        resultTextControl->Text = content;
    });

}


