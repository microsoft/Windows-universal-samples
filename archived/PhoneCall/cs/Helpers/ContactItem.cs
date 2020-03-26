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
using Windows.Storage.Streams;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace PhoneCall.Helpers
{
    class ContactItem
    {
        private string contactId;
        private string contactName;
        private ImageSource contactImage;

        /// <summary>
        /// A simple class that holds a contact item for the contacts panel.
        /// </summary>
        /// <param name="ContactId">The unique id of the contact from the contact store. Used to retrive the contact</param>
        /// <param name="contactName">The display name of the contact</param>
        public ContactItem(string ContactId, string ContactName)
        {
            this.contactId = ContactId;
            this.contactName = ContactName;
        }

        /// <summary>
        /// Sets the contact image. If there is no image, we set a default one picked from our solution
        /// </summary>
        /// <param name="ThumbnailReference">Thumbnail reference of the contact from the contact store.</param>
        public async void SetImageAsync(IRandomAccessStreamReference ThumbnailReference)
        {
            var thumbnailImage = new BitmapImage();
            if (ThumbnailReference != null)
            {
                using (IRandomAccessStreamWithContentType thumbnailStream = await ThumbnailReference.OpenReadAsync())
                {
                    thumbnailImage.SetSource(thumbnailStream);
                }
            }
            else
            {
                thumbnailImage = new BitmapImage(new Uri("ms-appx:///Assets/defaultContact.png", UriKind.Absolute));
            }
            this.contactImage = thumbnailImage;
        }

        #region Getters
        /// <summary>
        /// Gets the id of the contact.
        /// </summary>
        public string ContactId
        {
            get
            {
                return contactId;
            }
        }

        /// <summary>
        /// Gets the display name of the contact.
        /// </summary>
        public string ContactName
        {
            get
            {
                return contactName;
            }
        }

        /// <summary>
        /// Gets the image source of the contact.
        /// </summary>
        public ImageSource ContactImage
        {
            get
            {
                return contactImage;
            }
        }
        #endregion
    }
}
