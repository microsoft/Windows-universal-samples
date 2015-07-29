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
    public sealed partial class ShareWebLink
    {
        public ShareWebLink()
        {
            this.InitializeComponent();
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            // The URI used in this sample is provided by the user so we need to ensure it's a well formatted absolute URI
            // before we try to share it.
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            Uri dataPackageUri = ValidateAndGetUri(UriToShare.Text);
            if (dataPackageUri != null)
            {
                DataPackage requestData = request.Data;
                requestData.Properties.Title = TitleInputBox.Text;
                requestData.Properties.Description = DescriptionInputBox.Text; // The description is optional.
                requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                requestData.SetWebLink(dataPackageUri);
                succeeded = true;
            }
            else
            {
                request.FailWithDisplayText("Enter the web link you would like to share and try again.");
            }
            return succeeded;
        }

        private Uri ValidateAndGetUri(string uriString)
        {
            Uri uri = null;
            try
            {
                uri = new Uri(uriString);
            }
            catch (FormatException)
            {
                this.rootPage.NotifyUser("The web link provided is not a valid absolute URI.", NotifyType.ErrorMessage);
            }
            return uri;
        }
    }
}
