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

using System;
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class ShareCustomData
    {
        public ShareCustomData()
        {
            this.InitializeComponent();
            CustomDataTextBox.Text =
            @"{
               ""type"" : ""http://schema.org/Book"",
               ""properties"" :
               {
                ""image"" : ""http://sourceurl.com/catcher-in-the-rye-book-cover.jpg"",
                ""name"" : ""The Catcher in the Rye"",
                ""bookFormat"" : ""http://schema.org/Paperback"",
                ""author"" : ""http://sourceurl.com/author/jd_salinger.html"",
                ""numberOfPages"" : 224,
                ""publisher"" : ""Little, Brown, and Company"",
                ""datePublished"" : ""1991-05-01"",
                ""inLanguage"" : ""English"",
                ""isbn"" : ""0316769487""
                }
            }";
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            string dataPackageFormat = DataFormatInputBox.Text;
            if (!String.IsNullOrEmpty(dataPackageFormat))
            {
                string dataPackageText = CustomDataTextBox.Text;
                if (!String.IsNullOrEmpty(dataPackageText))
                {
                    DataPackage requestData = request.Data;
                    requestData.Properties.Title = TitleInputBox.Text;
                    requestData.Properties.Description = DescriptionInputBox.Text; // The description is optional.
                    requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                    requestData.SetData(dataPackageFormat, dataPackageText);
                    succeeded = true;
                }
                else
                {
                    request.FailWithDisplayText("Enter the custom data you would like to share and try again.");
                }
            }
            else
            {
                request.FailWithDisplayText("Enter a custom data format and try again.");
            }
            return succeeded;
        }
    }
}
