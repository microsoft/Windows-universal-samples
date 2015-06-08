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

//
// ShareWebLink.xaml.cpp
// Implementation of the ShareWebLink class
//

#include "pch.h"
#include "ShareWebLink.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::DataTransfer;

ShareWebLink::ShareWebLink()
{
    InitializeComponent();
}

bool ShareWebLink::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;

    // The URI used in this sample is provided by the user so we need to ensure it's a well formatted absolute URI
    // before we try to share it.
    MainPage::Current->NotifyUser("", NotifyType::StatusMessage);
    Uri^ dataPackageUri = ValidateAndGetUri(UriToShare->Text);
    if (dataPackageUri != nullptr)
    {
        auto requestData = request->Data;
        requestData->Properties->Title = TitleInputBox->Text;

        // The description is optional.
        auto dataPackageDescription = DescriptionInputBox->Text;
        if (dataPackageDescription != nullptr)
        {
            requestData->Properties->Description = dataPackageDescription;
        }
        requestData->SetWebLink(dataPackageUri);
        succeeded = true;
    }
    else
    {
        request->FailWithDisplayText("Enter the link you would like to share and try again.");
    }
    return succeeded;
}

Uri^ ShareWebLink::ValidateAndGetUri(String^ uriString)
{
    Uri^ uri = nullptr;
    try
    {
        uri = ref new Uri(uriString);
    }
    catch (InvalidArgumentException^)
    {
        MainPage::Current->NotifyUser("The link provided is not a valid absolute URI.", NotifyType::ErrorMessage);
    }
    catch (NullReferenceException^)
    {
        MainPage::Current->NotifyUser("Please provide a link to share.", NotifyType::ErrorMessage);
    }
    return uri;
}
