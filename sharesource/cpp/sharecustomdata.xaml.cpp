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
// ShareCustomData.xaml.cpp
// Implementation of the ShareCustomData class
//

#include "pch.h"
#include "ShareCustomData.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::DataTransfer;

ShareCustomData::ShareCustomData()
{
    InitializeComponent();
    CustomDataTextBox->Text =
        R"json({
    "type" : "http://schema.org/Book",
    "properties" :
    {
        "image" : "http://sourceurl.com/catcher-in-the-rye-book-cover.jpg",
        "name" : "The Catcher in the Rye",
        "bookFormat" : "http://schema.org/Paperback",
        "author" : "http://sourceurl.com/author/jd_salinger.html",
        "numberOfPages" : 224,
        "publisher" : "Little, Brown, and Company",
        "datePublished" : "1991-05-01",
        "inLanguage" : "English",
        "isbn" : "0316769487"
    }
})json";
}

bool ShareCustomData::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;

    auto dataPackageFormat = DataFormatInputBox->Text;
    if (dataPackageFormat != nullptr)
    {
        auto dataPackageText = CustomDataTextBox->Text;
        if (dataPackageText != nullptr)
        {
            auto requestData = request->Data;
            requestData->Properties->Title = TitleInputBox->Text;

            // The description is optional.
            auto dataPackageDescription = DescriptionInputBox->Text;
            if (dataPackageDescription != nullptr)
            {
                requestData->Properties->Description = dataPackageDescription;
            }
            requestData->SetData(dataPackageFormat, dataPackageText);
            succeeded = true;
        }
        else
        {
            request->FailWithDisplayText("Enter the custom data you would like to share and try again.");
        }
    }
    else
    {
        request->FailWithDisplayText("Enter a custom data format and try again.");
    }
    return succeeded;
}
