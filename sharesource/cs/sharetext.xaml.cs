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

namespace SDKTemplate
{
    public sealed partial class ShareText
    {
        public ShareText()
        {
            this.InitializeComponent();
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            string dataPackageText = TextToShare.Text;
            if (!String.IsNullOrEmpty(dataPackageText))
            {
                DataPackage requestData = request.Data;
                requestData.Properties.Title = TitleInputBox.Text;
                requestData.Properties.Description = DescriptionInputBox.Text; // The description is optional.
                requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                requestData.SetText(dataPackageText);
                succeeded = true;
            }
            else
            {
                request.FailWithDisplayText("Enter the text you would like to share and try again.");
            }
            return succeeded;
        }
    }
}
