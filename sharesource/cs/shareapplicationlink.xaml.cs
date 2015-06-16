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
using System.Collections.ObjectModel;
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Xaml.Controls;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml.Data;

namespace SDKTemplate
{
    public sealed partial class ShareApplicationLink
    {
        public ShareApplicationLink()
        {
            this.InitializeComponent();

            // Populate the ApplicationLinkComboBox with the deep links for all of the scenarios
            ObservableCollection<object> scenarioList = new ObservableCollection<object>();
            foreach (Scenario scenario in rootPage.scenarios)
            {
                scenarioList.Add(SharePage.GetApplicationLink(scenario.ClassType.Name));
            }
            ApplicationLinkComboBox.ItemsSource = scenarioList;
            ApplicationLinkComboBox.SelectedItem = ApplicationLink; // Default selection to the deep link for this scenario
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            Uri selectedApplicationLink = ApplicationLinkComboBox.SelectedItem as Uri;
            if (selectedApplicationLink != null)
            {
                DataPackage requestData = request.Data;
                requestData.Properties.Title = TitleInputBox.Text;
                requestData.Properties.Description = DescriptionInputBox.Text; // The description is optional.
                requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                requestData.SetApplicationLink(selectedApplicationLink);

                // Place the selected logo and the background color in the data package properties
                if (MicrosoftLogo.IsChecked.Value)
                {
                    requestData.Properties.Square30x30Logo = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///assets/microsoftLogo.png"));
                    requestData.Properties.LogoBackgroundColor = GetLogoBackgroundColor();
                }
                else if (VisualStudioLogo.IsChecked.Value)
                {
                    requestData.Properties.Square30x30Logo = RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///assets/visualStudioLogo.png"));
                    requestData.Properties.LogoBackgroundColor = GetLogoBackgroundColor();
                }

                succeeded = true;
            }
            else
            {
                request.FailWithDisplayText("Select the application link you would like to share and try again.");
            }

            return succeeded;
        }

        /// <summary>
        /// Reads out the values of the colors and constructs a color from their values.
        /// </summary>
        /// <returns></returns>
        private Color GetLogoBackgroundColor()
        {
            // The values are guaranteed to be within the appropriate byte range due to setting a minimum of 0
            // and a maximum of 255 on the sliders. However, it is necessary to convert them to bytes because
            // the slider exposes its value as a double.
            Color backgroundColor = new Color();
            backgroundColor.R = Convert.ToByte(RedSlider.Value);
            backgroundColor.G = Convert.ToByte(GreenSlider.Value);
            backgroundColor.B = Convert.ToByte(BlueSlider.Value);
            backgroundColor.A = Convert.ToByte(AlphaSlider.Value);

            return backgroundColor;
        }
    }

    /// <summary>
    /// A value converter which negates a bool. This converter is used for the color sliders to bind their enabled
    /// state to the negation of the selection state of the default logo radio button.
    /// </summary>
    public sealed class NegatingBoolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return !(bool)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return !(bool)value;
        }
    }
}
