//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.ViewManagement;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Storage.Pickers;
using Windows.Storage.Compression;
using Windows.ApplicationModel.Activation;

using System;
using System.Collections.Generic;
using System.Diagnostics;

using SDKTemplate;
using Windows.UI.Xaml.Data;
using System.Linq;

namespace Compression
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>

#if WINDOWS_PHONE_APP
    public sealed partial class Scenario1 : Page, IFileOpenPickerContinuable
#else
    public sealed partial class Scenario1 : Page
#endif
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>

#if WINDOWS_PHONE_APP

        private void ShowFileOpen(CompressAlgorithm? Algorithm)
        {
            var picker = new FileOpenPicker();

            picker.FileTypeFilter.Add("*");

            picker.ContinuationData["Operation"] = "CompressFile";
            picker.ContinuationData["CompressAlgorithm"] = Algorithm.ToString(); 

            picker.PickSingleFileAndContinue();
        }

        public void ContinueFileOpenPicker(FileOpenPickerContinuationEventArgs args)
        {
            if ((args.ContinuationData["Operation"] as string) == "CompressFile" &&
                args.Files != null &&
                args.Files.Count > 0)
            {
                StorageFile file = args.Files[0];
                string algorithmRaw = args.ContinuationData["CompressAlgorithm"] as string;

                CompressAlgorithm? Algorithm;

                if (algorithmRaw == "")
                {
                    Algorithm = null;
                }
                else
                {
                    Algorithm = (CompressAlgorithm?)Enum.Parse(typeof(CompressAlgorithm), algorithmRaw);
                }   

                CompressFile(Algorithm, file);
            }
        }
#endif
        private async void CompressFile(CompressAlgorithm? Algorithm, StorageFile originalFile)
        {
            Progress.Text = "";
            
            try
            {
                Progress.Text += String.Format("\"{0}\" has been picked\n", originalFile.Name);

                var compressedFilename = originalFile.Name + ".compressed";
                var compressedFile = await ApplicationData.Current.TemporaryFolder.CreateFileAsync(compressedFilename, CreationCollisionOption.GenerateUniqueName);
                Progress.Text += String.Format("\"{0}\" has been created to store compressed data\n", compressedFile.Path);

                // ** DO COMPRESSION **
                // Following code actually performs compression from original file to the newly created
                // compressed file. In order to do so it:
                // 1. Opens input for the original file.
                // 2. Opens output stream on the file to be compressed and wraps it into Compressor object.
                // 3. Copies original stream into Compressor wrapper.
                // 4. Finalizes compressor - it puts termination mark into stream and flushes all intermediate
                //    buffers.
                using (var originalInput = await originalFile.OpenReadAsync())
                using (var compressedOutput = await compressedFile.OpenAsync(FileAccessMode.ReadWrite))
                using (var compressor = !Algorithm.HasValue ?
                    new Compressor(compressedOutput.GetOutputStreamAt(0)) :
                    new Compressor(compressedOutput.GetOutputStreamAt(0), Algorithm.Value, 0))
                {
                    Progress.Text += "All streams wired for compression\n";
                    var bytesCompressed = await RandomAccessStream.CopyAsync(originalInput, compressor);
                    var finished = await compressor.FinishAsync();
                    Progress.Text += String.Format("Compressed {0} bytes into {1}\n", bytesCompressed, compressedOutput.Size);
                }

                var decompressedFilename = originalFile.Name + ".decompressed";
                var decompressedFile = await ApplicationData.Current.TemporaryFolder.CreateFileAsync(decompressedFilename, CreationCollisionOption.GenerateUniqueName);
                Progress.Text += String.Format("\"{0}\" has been created to store decompressed data\n", decompressedFile.Path);

                // ** DO DECOMPRESSION **
                // Following code performs decompression from the just compressed file to the
                // decompressed file. In order to do so it:
                // 1. Opens input stream on compressed file and wraps it into Decompressor object.
                // 2. Opens output stream from the file that will store decompressed data.
                // 3. Copies data from Decompressor stream into decompressed file stream.
                using (var compressedInput = await compressedFile.OpenSequentialReadAsync())
                using (var decompressor = new Decompressor(compressedInput))
                using (var decompressedOutput = await decompressedFile.OpenAsync(FileAccessMode.ReadWrite))
                {
                    Progress.Text += "All streams wired for decompression\n";
                    var bytesDecompressed = await RandomAccessStream.CopyAsync(decompressor, decompressedOutput);
                    Progress.Text += String.Format("Decompressed {0} bytes of data\n", bytesDecompressed);
                }

                rootPage.NotifyUser("All done", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the main scenario worker.
        /// </summary>
        /// <param name="Algorithm">
        /// Comression algorithm to use. If no value is provided compressor will be created using
        /// Compressor(IInputStream) constructor, otherwise extended version will be used:
        /// Compressor(IInputStream, CompressAlgorithm, uint)
        /// </param>
#if WINDOWS_PHONE_APP
        private void DoScenario(CompressAlgorithm? Algorithm)
#else
        private async void DoScenario(CompressAlgorithm? Algorithm)
#endif
        {
            try
            {
                rootPage.NotifyUser("Working...", NotifyType.StatusMessage);

#if WINDOWS_PHONE_APP
                ShowFileOpen(Algorithm);
#else
                StorageFile originalFile;

                var picker = new FileOpenPicker();

                picker.FileTypeFilter.Add("*");

                originalFile = await picker.PickSingleFileAsync();
                if (originalFile == null)
                {
                    throw new OperationCanceledException("No file has been selected");
                }

                CompressFile(Algorithm, originalFile);
#endif
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void CompressAlgorithmComboBox_SelectionChanged(object sender, RoutedEventArgs e)
        {
            var args = e as SelectionChangedEventArgs;

            if (args == null)
            {
                return;                    
            }
            if (CompressAlgorithmComboBox == null)
            {
                return;
            }

            if (args.AddedItems.Count != 0 && args.RemovedItems.Count == 0)
            {
                ComboBoxItem addedItem = args.AddedItems[0] as ComboBoxItem;
                if (addedItem.Content.Equals("Default"))
                {
                    UpdateDescriptionVisibility(addedItem, Visibility.Visible);
                }

                return;
            }
            else
            {
                Debug.Assert(1 == args.AddedItems.Count && 1 == args.RemovedItems.Count);

                ComboBoxItem addedItem = args.AddedItems[0] as ComboBoxItem;
                ComboBoxItem removedItem = args.RemovedItems[0] as ComboBoxItem;

                Debug.Assert(null != addedItem && null != removedItem);

                UpdateDescriptionVisibility(addedItem, Visibility.Visible);
                UpdateDescriptionVisibility(removedItem, Visibility.Collapsed);
            }
        }

        private CompressAlgorithm? CompressAlgorithmFromComboBoxItem(ComboBoxItem item)
        {
            CompressAlgorithm? Algorithm = null;

            if (item == DefaultComboBoxItem)
            {
                Algorithm = null;
            }
            else if (item == XpressComboBoxItem)
            {
                Algorithm = CompressAlgorithm.Xpress;
            }
            else if (item == XpressHuffComboBoxItem)
            {
                Algorithm = CompressAlgorithm.XpressHuff;
            }
            else if (item == MszipComboBoxItem)
            {
                Algorithm = CompressAlgorithm.Mszip;
            }
            else if (item == LzmsComboBoxItem)
            {
                Algorithm = CompressAlgorithm.Lzms;
            }
            else 
            {
                Algorithm = CompressAlgorithm.InvalidAlgorithm;
            }

            return Algorithm;
        }

        private void UpdateDescriptionVisibility(ComboBoxItem item, Visibility visibility)
        {
            TextBlock textBlock = null;

            CompressAlgorithm? Algorithm = CompressAlgorithmFromComboBoxItem(item);

            switch (Algorithm)
            {
            case null:
                textBlock = DefaultTextBlock;
                break;
            case CompressAlgorithm.Xpress:
                textBlock = XpressTextBlock;
                break;
            case CompressAlgorithm.XpressHuff:
                textBlock = XpressHuffTextBlock;
                break;
            case CompressAlgorithm.Mszip:
                textBlock = MszipTextBlock;
                break;
            case CompressAlgorithm.Lzms:
                textBlock = LzmsTextBlock;
                break;
            default:
                return;
            }

            textBlock.Visibility = visibility;
        }

        private void CompressFileButton_Click(object sender, RoutedEventArgs e)
        {
            ComboBoxItem item = CompressAlgorithmComboBox.SelectedItem as ComboBoxItem;

            if (item == null)
            {
                return;
            }

            CompressAlgorithm? Algorithm = CompressAlgorithmFromComboBoxItem(item);

            if (Algorithm == CompressAlgorithm.InvalidAlgorithm)
            {
                return;
            }

            DoScenario(Algorithm);
        }
    }
}
