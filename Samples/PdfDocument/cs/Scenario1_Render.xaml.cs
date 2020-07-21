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
using System.Threading.Tasks;
using Windows.Data.Pdf;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Render : Page
    {
        private MainPage rootPage = MainPage.Current;
        private PdfDocument pdfDocument;

        const int WrongPassword = unchecked((int)0x8007052b); // HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD)
        const int GenericFail = unchecked((int)0x80004005);   // E_FAIL

        public Scenario1_Render()
        {
            this.InitializeComponent();
        }

        private async void LoadDocument()
        {
            LoadButton.IsEnabled = false;

            pdfDocument = null;
            Output.Source = null;
            PageNumberBox.Text = "1";
            RenderingPanel.Visibility = Visibility.Collapsed;

            var picker = new FileOpenPicker();
            picker.FileTypeFilter.Add(".pdf");
            StorageFile file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                ProgressControl.Visibility = Visibility.Visible;
                try
                {
                    pdfDocument = await PdfDocument.LoadFromFileAsync(file, PasswordBox.Password);
                }
                catch (Exception ex)
                {
                    switch (ex.HResult)
                    {
                        case WrongPassword:
                            rootPage.NotifyUser("Document is password-protected and password is incorrect.", NotifyType.ErrorMessage);
                            break;

                        case GenericFail:
                            rootPage.NotifyUser("Document is not a valid PDF.", NotifyType.ErrorMessage);
                            break;

                        default:
                            // File I/O errors are reported as exceptions.
                            rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                            break;
                    }
                }

                if (pdfDocument != null)
                {
                    RenderingPanel.Visibility = Visibility.Visible;
                    if (pdfDocument.IsPasswordProtected)
                    {
                        rootPage.NotifyUser("Document is password protected.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Document is not password protected.", NotifyType.StatusMessage);
                    }
                    PageCountText.Text = pdfDocument.PageCount.ToString();
                }
                ProgressControl.Visibility = Visibility.Collapsed;
            }
            LoadButton.IsEnabled = true;
        }

        private async void ViewPage()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            uint pageNumber;
            if (!uint.TryParse(PageNumberBox.Text, out pageNumber) || (pageNumber < 1) || (pageNumber > pdfDocument.PageCount))
            {
                rootPage.NotifyUser("Invalid page number.", NotifyType.ErrorMessage);
                return;
            }

            Output.Source = null;
            ProgressControl.Visibility = Visibility.Visible;

            // Convert from 1-based page number to 0-based page index.
            uint pageIndex = pageNumber - 1;

            using (PdfPage page = pdfDocument.GetPage(pageIndex))
            {
                var stream = new InMemoryRandomAccessStream();
                switch (Options.SelectedIndex)
                {
                    // View actual size.
                    case 0:
                        await page.RenderToStreamAsync(stream);
                        break;

                    // View half size on beige background.
                    case 1:
                        var options1 = new PdfPageRenderOptions();
                        options1.BackgroundColor = Windows.UI.Colors.Beige;
                        options1.DestinationHeight = (uint)(page.Size.Height / 2);
                        options1.DestinationWidth = (uint)(page.Size.Width / 2);
                        await page.RenderToStreamAsync(stream, options1);
                        break;

                    // Crop to center.
                    case 2:
                        var options2 = new PdfPageRenderOptions();
                        var rect = page.Dimensions.TrimBox;
                        options2.SourceRect = new Rect(rect.X + rect.Width / 4, rect.Y + rect.Height / 4, rect.Width / 2, rect.Height / 2);
                        await page.RenderToStreamAsync(stream, options2);
                        break;
                }
                BitmapImage src = new BitmapImage();
                Output.Source = src;
                await src.SetSourceAsync(stream);
            }
            ProgressControl.Visibility = Visibility.Collapsed;
        }
    }
}
