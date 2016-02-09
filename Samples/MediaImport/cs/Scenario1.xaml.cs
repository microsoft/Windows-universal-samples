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

using DataBinding;
using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Windows.ApplicationModel.Activation;
using Windows.Media.Import;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System.ComponentModel;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace ImportSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page , INotifyPropertyChanged
    {
        private MainPage rootPage;
        private PhotoImportSession session;
        private PhotoImportSource importSource;
        private PhotoImportFindItemsResult itemsResult;
        private PhotoImportImportItemsResult importedResult;
        private PhotoImportDeleteImportedItemsFromSourceResult deleteResult;
        private CancellationTokenSource cts;
        private DeviceActivatedEventArgs arguments;
        private PhotoImportContentTypeFilter contentTypeFilder;

        // GeneratorIncrementalLoadingClass taken from MSDN example https://code.msdn.microsoft.com/windowsapps/Data-Binding-7b1d67b5 
        // Used to incrementally load items in the Listview view including thumbnails
        GeneratorIncrementalLoadingClass<ImportableItemWrapper> itemsToImport = null;

        public Scenario1()
        {
            this.InitializeComponent();
            this.DataContext = this;
        }


        public bool AppendSessionDateToDestinationFolder
        {
            get
            {
                if (session != null)
                {
                    return session.AppendSessionDateToDestinationFolder;
                }
                else
                {
                    return false;
                }
            }
            set
            {
                if (session != null)
                {
                    session.AppendSessionDateToDestinationFolder = value;
                    RaisePropertyChanged("AppendSessionDateToDestinationFolder");
                }
            }
        }

        /// <summary>
        /// Enumerate all PTP / MTP sources when FindAllSources button is clicked. 
        /// </summary>
        /// <param name="sender">The source of the click event</param>
        /// <param name="e">Details about the click event </param>
        private async void OnFindSourcesClicked(object sender, RoutedEventArgs e)
        {
            try
            {
                rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage);

                // clear our list of items to import
                this.itemsToImport = null;
                this.fileListView.ItemsSource = this.itemsToImport;

                // reset UI elements
                this.progressBar.Value = 0.0;
                this.propertiesTxtBox.Text = "";

                // disable all buttons until source selection is made
                setButtonState(false);

                // reenable source selection
                this.sourceListBox.IsEnabled = true;

                // clear find criteria
                this.OnlyImages.IsChecked = false;
                this.OnlyVideo.IsChecked = false;
                this.ImagesAndVideo.IsChecked = false;
                


                // Dispose any created import session
                if (this.session != null)
                {
                    this.session.Dispose();
                    this.session = null;
                }

                // Check to ensure API is supported
                bool importSupported = await PhotoImportManager.IsSupportedAsync();

                if (importSupported)
                {
                    // enumerate all available sources to where we can import from
                    sourceListBox.ItemsSource = await PhotoImportManager.FindAllSourcesAsync();
                }
                else
                {
                    rootPage.NotifyUser("Import is not supported.", NotifyType.ErrorMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("FindAllSourcesAsync Failed. " + "Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Display the source properties when a new source is selected.
        /// </summary>
        /// <param name="sender">The source of the selection changed event</param>
        /// <param name="e">Details about the selection chagned event</param>
        private void  SourceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // reset any previous items
            this.itemsToImport = null;
            this.fileListView.ItemsSource = null;

            // clear any values in the source properties
            this.propertiesTxtBox.Text = "";

            if (e.AddedItems.Count != 0)
            {
                // enable find options
                this.OnlyImages.IsEnabled = true;
                this.OnlyVideo.IsEnabled = true;
                this.ImagesAndVideo.IsEnabled = true;

                // set the default to find Images and Video
                this.ImagesAndVideo.IsChecked = true;

                this.importSource = e.AddedItems.First() as PhotoImportSource;
                DisplaySourceProperties();
                    
                // enable Find New and Find All buttons, disable the rest
                this.findNewItemsButton.IsEnabled = true;
                this.findAllItemsButton.IsEnabled = true;
                this.selectNewButton.IsEnabled = false;
                this.selectNoneButton.IsEnabled = false;
                this.selectAllButton.IsEnabled = false;
                this.deleteButton.IsEnabled = false;
                this.importButton.IsEnabled = false;
            }
        }

        /// <summary>
        /// Enumerate new importable items from the source when FindAllItems button is clicked
        /// </summary>
        /// <param name="sender">The source of the click event</param>
        /// <param name="e">Details about the click event</param>
        private void OnFindNewItemsClicked(object sender, RoutedEventArgs e)
        {
            findItems(this.contentTypeFilder, PhotoImportItemSelectionMode.SelectNew);
        }

        /// <summary>
        /// Enumerate all importable items from the source when FindAllItems button is clicked
        /// </summary>
        /// <param name="sender">The source of the click event</param>
        /// <param name="e">Details about the click event</param>
        private void OnFindAllItemsClicked(object sender, RoutedEventArgs e)
        {
            findItems(this.contentTypeFilder, PhotoImportItemSelectionMode.SelectAll);           
        }

        /// <summary>
        /// Helper method to Enumerate all importable items
        /// </summary>
        /// <param name="contentType">the content type</param>
        /// <param name="selectionMode">the selection mode</param>
        private async void findItems(PhotoImportContentTypeFilter contentType, PhotoImportItemSelectionMode selectionMode)
        {
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage);

            // Disable buttons and lists to prevent unexpected reentrance.
            setButtonState(false);
            this.sourceListBox.IsEnabled = false;

            this.cts = new CancellationTokenSource();

            try
            {
                this.progressRing.IsActive = true;
                this.itemsToImport = null;
                this.fileListView.ItemsSource = null;

                // Dispose any created import session
                if (this.session != null)
                {
                    this.session.Dispose();
                    this.session = null;
                }

                // Create the import session from the source selected
                this.session = importSource.CreateImportSession();

                RaisePropertyChanged("AppendSessionDateToDestinationFolder");

                // Progress handler for FindItemsAsync
                var progress = new Progress<uint>((result) =>
                {
                    rootPage.NotifyUser(String.Format("Found {0} Files", result.ToString()), NotifyType.StatusMessage);
                });

                // Find all items available importable item from our source
                this.itemsResult = await session.FindItemsAsync(contentType, selectionMode).AsTask(cts.Token, progress);

                setIncrementalFileList(this.itemsResult);
                DisplayFindItemsResult();

                if (this.itemsResult.HasSucceeded)
                {
                    // disable Find New , Find All and Delete button. 
                    this.findNewItemsButton.IsEnabled = false;
                    this.findAllItemsButton.IsEnabled = false;
                    this.deleteButton.IsEnabled = false;
                    
                    // enable list selection , folder options and import buttons
                    this.selectNewButton.IsEnabled = true;
                    this.selectNoneButton.IsEnabled = true;
                    this.selectAllButton.IsEnabled = true;
                    this.importButton.IsEnabled = true;
                    this.fileListView.IsEnabled = true;
                    this.AddSessionDateFolder.IsEnabled = true;
                    this.NoSubFolder.IsEnabled = true;
                    this.FileDateSubfolder.IsEnabled = true;
                    this.ExifDateSubfolder.IsEnabled = true;
                    this.KeepOriginalFolder.IsEnabled = true;

                }
                else
                {                    
                    rootPage.NotifyUser("FindItemsAsync did not succeed or was not completed.", NotifyType.StatusMessage);
                    
                    // re-enable Find New and Find All buttons
                    this.findNewItemsButton.IsEnabled = true;
                    this.findAllItemsButton.IsEnabled = true;

                    this.progressRing.IsActive = false;
                }

                this.progressRing.IsActive = false;
                
                // Set the CancellationTokenSource to null when the work is complete.
                cts = null;
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error: Operation incomplete. " + "Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
        }


        /// <summary>
        /// Initiate the import operation when the Import button is clicked.
        /// </summary>
        /// <param name="sender">Source of the click event</param>
        /// <param name="e">Details about the click event</param>
        private async void OnImportButtonClicked(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage);

            // disable UI while async operations are running.
            setButtonState(false);
            this.sourceListBox.IsEnabled = false;
            this.progressBar.Value = 0.0;

            cts = new CancellationTokenSource();

            try
            {
                if (itemsResult.SelectedTotalCount <= 0)
                {
                    rootPage.NotifyUser("Nothing Selected for Import.", NotifyType.StatusMessage);
                }
                else
                {                    
                    var progress = new Progress<PhotoImportProgress>((result) =>
                    {
                        progressBar.Value =  result.ImportProgress;
                    });         
                               
                    this.itemsResult.ItemImported += async (s, a) =>
                    {                        
                        await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            rootPage.NotifyUser(String.Format("Imported: {0}", a.ImportedItem.Name), NotifyType.StatusMessage);
                        });
                    };

                    // import items from the our list of selected items
                    this.importedResult = await this.itemsResult.ImportItemsAsync().AsTask(cts.Token, progress);

                    DisplayImportedSummary();

                    if (!this.importedResult.HasSucceeded)
                    {
                        rootPage.NotifyUser("ImportItemsAsync did not succeed or was not completed", NotifyType.StatusMessage);
                    }
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Files could not be imported. " + "Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }

            cts = null;

            // allow the user to find sources and delete the files from the media
            this.findSourceButton.IsEnabled = true;
            this.deleteButton.IsEnabled = true;

            this.importButton.IsEnabled = false;
            this.findNewItemsButton.IsEnabled = false;
            this.findAllItemsButton.IsEnabled = false;
            this.selectNewButton.IsEnabled = false;
            this.selectAllButton.IsEnabled = false;
            this.selectNoneButton.IsEnabled = false;

            this.NoSubFolder.IsChecked = false;
            this.FileDateSubfolder.IsChecked = false;
            this.ExifDateSubfolder.IsChecked = false;
            this.KeepOriginalFolder.IsChecked = false;
        }

        /// <summary>
        /// Select all items.
        /// </summary>
        /// <param name="sender">Source of the click event</param>
        /// <param name="e">Details about the click event</param>
        private void OnSelectAllButtonClicked(object sender, RoutedEventArgs e)
        {
            // select all items for import        
            this.itemsResult.SelectAll();
            setIncrementalFileList(this.itemsResult);
            DisplayFindItemsResult();
        }

        /// <summary>
        /// Select only new items. Items that have already been imported is not selected.
        /// </summary>
        /// <param name="sender">Source of the click event</param>
        /// <param name="e">Details about the click event</param>
        private async void OnSelectNewButtonClicked(object sender, RoutedEventArgs e)
        {
            // select only items that new
            await this.itemsResult.SelectNewAsync();
            setIncrementalFileList(this.itemsResult);
            DisplayFindItemsResult();
        }

        /// <summary>
        /// Deselect all items.
        /// </summary>
        /// <param name="sender">Source of the click event</param>
        /// <param name="e">Details about the click event</param>
        private void OnSelectNoneButtonClicked(object sender, RoutedEventArgs e)
        { 
            // deselects all items
            this.itemsResult.SelectNone();
            setIncrementalFileList(this.itemsResult);
            DisplayFindItemsResult();
        }

        /// <summary>
        /// Creates a GeneratorIncrementalLoadingClass and sets the FindItemsResult to the filelist for display
        /// </summary>
        /// <param name="list">PhotoImportFindItemResult list</param>
        private void setIncrementalFileList (PhotoImportFindItemsResult list)
        {
            // GeneratorIncrementalLoadingClass is used to incrementally load items in the Listview view including thumbnails
            this.itemsToImport = new GeneratorIncrementalLoadingClass<ImportableItemWrapper>(list.TotalCount,
            (int index) =>
            {
                return new ImportableItemWrapper(list.FoundItems[index]);
            });
            this.fileListView.ItemsSource = this.itemsToImport;
        } 


        /// <summary>
        /// When a user clicks the delete button, deletes the content that were imported.
        /// Calling DeleteImportedItemsFromSourceAsync does not delete content that were not imported.
        /// </summary>
        /// <param name="sender">Source of the click event</param>
        /// <param name="e">Detials about the click event</param>
        private async void OnDeleteButtonClicked(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Deleting items...", NotifyType.StatusMessage);

            // Prevent unexpected reentrance.
            this.deleteButton.IsEnabled = false;

            cts = new CancellationTokenSource();

            try
            {                
                if (importedResult == null)
                {
                    rootPage.NotifyUser("Nothing was imported for deletion.", NotifyType.StatusMessage);
                }
                else
                {
                    var progress = new Progress<double>((result) =>
                    {
                        this.progressBar.Value = result;
                    });

                    this.deleteResult = await this.importedResult.DeleteImportedItemsFromSourceAsync().AsTask(cts.Token, progress);

                    DisplayDeletedResults();

                    if (!this.deleteResult.HasSucceeded)
                    {
                        rootPage.NotifyUser("Delete operation did not succeed or was not completed", NotifyType.StatusMessage);
                    }
                }
                
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Files could not be Deleted." + "Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }

            // set the CancellationTokenSource to null when the work is complete.
            cts = null;


            // content is removed, disable the appropriate buttons 

            this.findSourceButton.IsEnabled = true;
            this.importButton.IsEnabled = false;
            this.findNewItemsButton.IsEnabled = false;
            this.findAllItemsButton.IsEnabled = false;
            this.selectNewButton.IsEnabled = false;
            this.selectAllButton.IsEnabled = false;
            this.selectNoneButton.IsEnabled = false;
            this.deleteButton.IsEnabled = false;
            this.fileListView.IsEnabled = false;
        }

        /// <summary>
        /// Cancels an async operation
        /// </summary>
        /// <param name="sender">Source of the cancel event</param>
        /// <param name="e">Details about the cancel event</param>
        private void OnCancelButtonClicked(object sender, RoutedEventArgs e)
        {
            if (cts != null)
            {
                cts.Cancel();
                rootPage.NotifyUser("Operation canceled by the Cancel button.", NotifyType.StatusMessage);
            }
        }
        
        /// <summary>
        /// Helper method to display the files enumerated from the source.
        /// </summary>
        void DisplayFindItemsResult()
        {
            if (itemsResult != null)
            {
                StringBuilder findResultProperties = new StringBuilder();
                findResultProperties.AppendLine(String.Format("Photos\t\t\t :  {0} \t\t Selected Photos\t\t:  {1}", itemsResult.PhotosCount, itemsResult.SelectedPhotosCount));
                findResultProperties.AppendLine(String.Format("Videos\t\t\t :  {0} \t\t Selected Videos\t\t:  {1}", itemsResult.VideosCount, itemsResult.SelectedVideosCount));
                findResultProperties.AppendLine(String.Format("SideCars\t\t :  {0} \t\t Selected Sidecars\t:  {1}", itemsResult.SidecarsCount, itemsResult.SelectedSidecarsCount));
                findResultProperties.AppendLine(String.Format("Siblings\t\t\t :  {0} \t\t Selected Sibilings\t:  {1} ", itemsResult.SiblingsCount, itemsResult.SelectedSiblingsCount));
                findResultProperties.AppendLine(String.Format("Total Items Items\t :  {0} \t\t Selected TotalCount \t:  {1}", itemsResult.TotalCount, itemsResult.SelectedTotalCount));

                rootPage.NotifyUser(findResultProperties.ToString(), NotifyType.StatusMessage);
            }
        }

        /// <summary>
        ///  Helper method to display the files that were imported.
        /// </summary>
        void DisplayImportedSummary()
        {
            if (importedResult != null)
            {
                StringBuilder importedSummary = new StringBuilder();
                importedSummary.AppendLine(String.Format("Photos Imported   \t:  {0} ", importedResult.PhotosCount));
                importedSummary.AppendLine(String.Format("Videos Imported    \t:  {0} ", importedResult.VideosCount));
                importedSummary.AppendLine(String.Format("SideCars Imported   \t:  {0} ", importedResult.SidecarsCount));
                importedSummary.AppendLine(String.Format("Siblings Imported   \t:  {0} ", importedResult.SiblingsCount));
                importedSummary.AppendLine(String.Format("Total Items Imported \t:  {0} ", importedResult.TotalCount));
                importedSummary.AppendLine(String.Format("Total Bytes Imported \t:  {0} ", importedResult.TotalSizeInBytes));

                rootPage.NotifyUser(importedSummary.ToString(), NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Helper method to display the source properties information.
        /// </summary>
        void DisplaySourceProperties()
        {
            if (importSource != null)
            {
                StringBuilder sourceProperties = new StringBuilder();
                sourceProperties.AppendLine(String.Format("Description:         {0} ", importSource.Description));
                sourceProperties.AppendLine(String.Format("IsMassStorage:         {0} ", importSource.IsMassStorage));
                sourceProperties.AppendLine(String.Format("Manufacturer:          {0} ", importSource.Manufacturer));                

                IReadOnlyList<PhotoImportStorageMedium> sm = importSource.StorageMedia;
                sourceProperties.AppendLine(String.Format("AvailableSpaceInBytes:         {0} ", sm[0].AvailableSpaceInBytes));
                sourceProperties.AppendLine(String.Format("CapacityInBytes:         {0} ", sm[0].CapacityInBytes));
                sourceProperties.AppendLine(String.Format("Name:         {0} ", sm[0].Name));
                sourceProperties.AppendLine(String.Format("SerialNumber:         {0} ", sm[0].SerialNumber));
                sourceProperties.AppendLine(String.Format("StorageMediaType:         {0} ", sm[0].StorageMediumType));
                sourceProperties.AppendLine(String.Format("Id:         {0} ", importSource.Id));

                propertiesTxtBox.Text = sourceProperties.ToString();
            }
        }

        /// <summary>
        /// Helper method to display the imported files that were deleted
        /// </summary>
        void DisplayDeletedResults()
        {
            if (deleteResult != null)
            {
                StringBuilder deletedResults = new StringBuilder();
                deletedResults.AppendLine(String.Format("Total Photos Deleted:\t{0} ", deleteResult.PhotosCount));
                deletedResults.AppendLine(String.Format("Total Videos Deleted:\t{0} ", deleteResult.VideosCount));
                deletedResults.AppendLine(String.Format("Total Sidecars Deleted:\t{0} ", deleteResult.SidecarsCount));
                deletedResults.AppendLine(String.Format("Total Sibilings Deleted:\t{0} ", deleteResult.SiblingsCount));
                deletedResults.AppendLine(String.Format("Total Files Deleted:\t{0} ", deleteResult.TotalCount));
                deletedResults.AppendLine(String.Format("Total Bytes Deleted:\t{0} ", deleteResult.TotalSizeInBytes));
                rootPage.NotifyUser(deletedResults.ToString(), NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Helper method to set the state of the button
        /// </summary>
        /// <param name="isEnabled">state of the button</param>
        void setButtonState(bool isEnabled)
        {
            this.importButton.IsEnabled = isEnabled;
            this.deleteButton.IsEnabled = isEnabled;
            this.findNewItemsButton.IsEnabled = isEnabled;
            this.findAllItemsButton.IsEnabled = isEnabled;
            this.selectNewButton.IsEnabled = isEnabled;
            this.selectAllButton.IsEnabled = isEnabled;
            this.selectNoneButton.IsEnabled = isEnabled;
            this.fileListView.IsEnabled = isEnabled;

            this.OnlyImages.IsEnabled = isEnabled;
            this.OnlyVideo.IsEnabled = isEnabled;
            this.ImagesAndVideo.IsEnabled = isEnabled;
            this.AddSessionDateFolder.IsEnabled = isEnabled;
            this.NoSubFolder.IsEnabled = isEnabled;
            this.FileDateSubfolder.IsEnabled = isEnabled;
            this.ExifDateSubfolder.IsEnabled = isEnabled;
            this.KeepOriginalFolder.IsEnabled = isEnabled;
        }

        /// <summary>
        /// When a checkbox is clicked, update the UI to the latest selection  
        /// </summary>
        /// <param name="sender">Sender</param>
        /// <param name="e">RoutedEventArgs</param>
        private void CheckBox_Click(object sender, RoutedEventArgs e)
        {
            // update the result count 
            DisplayFindItemsResult();
        }

        /// <summary
        /// This app is registered as a hanlder for WPD\IMageSourceAutoPlay event.
        /// When a user connects a WPD device (ie: Camera), OnNavigatedTo is called with DeviceInformationId
        /// that we can call FromIdAsync() and preselect the device.
        /// 
        /// We also need to handle the situation where the client was terminated while an import operation was in flight.
        /// Import will continue and we attempt to reconnect back to the session.
        /// </summary>
        /// <param name="e">Details about the NavigationEventArgs</param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            rootPage = MainPage.Current;
            try
            {
                //An photo import session was in progress
                if (e.Parameter is PhotoImportOperation)
                {
                    PhotoImportOperation op = e.Parameter as PhotoImportOperation;
                    switch (op.Stage)
                    {
                        // file enumeration was running
                        case PhotoImportStage.FindingItems:

                            cts = new CancellationTokenSource();
                            rootPage.NotifyUser("Reconnected back to the file enumeration stage", NotifyType.StatusMessage);

                            // Set up progress handler for existing file enumeration
                            var findAllItemsProgress = new Progress<uint>((result) =>
                            {
                                rootPage.NotifyUser(String.Format("Found {0} Files", result.ToString()), NotifyType.StatusMessage);
                            });

                            // retrieve previous session
                            this.session = op.Session;
                            // get to the operation for the existing FindItemsAsync session
                            this.itemsResult = await op.ContinueFindingItemsAsync.AsTask(cts.Token, findAllItemsProgress);

                            // display the items we found in the previous session
                            setIncrementalFileList(this.itemsResult);
                            this.selectAllButton.IsEnabled = true;
                            this.selectNoneButton.IsEnabled = true;
                            this.selectNewButton.IsEnabled = true;
                            this.importButton.IsEnabled = true;

                            DisplayFindItemsResult();

                            cts = null;
                            break;

                        // Import was running
                        case PhotoImportStage.ImportingItems:

                            rootPage.NotifyUser("Reconnected back to the importing stage.", NotifyType.StatusMessage);

                            setButtonState(false);
                            cts = new CancellationTokenSource();

                            // Set up progress handler for the existing import session

                            var importSelectedItemsProgress = new Progress<PhotoImportProgress>((result) =>
                            {
                                progressBar.Value = result.ImportProgress;
                            });

                            // retrieve the previous operation
                            this.session = op.Session;
                            
                            // get the itemsResult that were found in the previous session
                            this.itemsResult = await op.ContinueFindingItemsAsync.AsTask();
                            
                            // hook up the ItemImported Handler to show progress
                            this.itemsResult.ItemImported += async (s, a) =>
                            {
                                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                                {
                                    rootPage.NotifyUser(String.Format("Imported: {0}", a.ImportedItem.Name), NotifyType.StatusMessage);
                                });
                            };

                            // Get the operation for the import operation that was in flight
                            this.importedResult = await op.ContinueImportingItemsAsync.AsTask(cts.Token, importSelectedItemsProgress);

                            DisplayImportedSummary();

                            this.findSourceButton.IsEnabled = true;
                            this.deleteButton.IsEnabled = true;

                            cts = null;
                            break;

                        // Delete operation is in progress
                        case PhotoImportStage.DeletingImportedItemsFromSource:

                            rootPage.NotifyUser("Reconnected to the deletion stage.",NotifyType.StatusMessage);

                            this.findSourceButton.IsEnabled = false;
                            this.deleteButton.IsEnabled = false;
                            cts = new CancellationTokenSource();
                                                       
                            var progress = new Progress<double>((result) =>
                            {
                                this.progressBar.Value = result;
                            });

                            // get the operation for the existing delete session
                            this.deleteResult = await op.ContinueDeletingImportedItemsFromSourceAsync.AsTask(cts.Token, progress);

                            DisplayDeletedResults();

                            if (!deleteResult.HasSucceeded)
                            {
                                rootPage.NotifyUser("Deletion did not succeeded or was cancelled.", NotifyType.StatusMessage);
                            }

                            this.findSourceButton.IsEnabled = true;

                            // Set the CancellationTokenSource to null when the work is complete.
                            cts = null;
                            break;

                        // Idle State.
                        case PhotoImportStage.NotStarted:

                            rootPage.NotifyUser("No import tasks was started.", NotifyType.StatusMessage);
                            this.session = op.Session;
                            break;

                        default:
                            break;
                    }
                }

                // Activated by AutoPlay
                if (e.Parameter is DeviceActivatedEventArgs)
                {
                    this.arguments = e.Parameter as DeviceActivatedEventArgs;
                    bool importSupported = await PhotoImportManager.IsSupportedAsync();
                    this.sourceListBox.Items.Clear();

                    List<PhotoImportSource> source = new List<PhotoImportSource>();
                    
                    // Create the source from a device Id
                    source.Add(await PhotoImportSource.FromIdAsync(this.arguments.DeviceInformationId));

                    // bind and preselects source
                    this.sourceListBox.ItemsSource = source;
                    this.sourceListBox.SelectedIndex = 0;

                    rootPage.NotifyUser("Device selected from AutoPlay", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void OnlyImages_Checked(object sender, RoutedEventArgs e)
        {
            this.contentTypeFilder = PhotoImportContentTypeFilter.OnlyImages;

            // enable find since the filter has changed
            this.findAllItemsButton.IsEnabled = true;
            this.findNewItemsButton.IsEnabled = true;
        }

        private void OnlyVideo_Checked(object sender, RoutedEventArgs e)
        {
            this.contentTypeFilder = PhotoImportContentTypeFilter.OnlyVideos;

            // enable find since the filter has changed
            this.findAllItemsButton.IsEnabled = true;
            this.findNewItemsButton.IsEnabled = true;
        }

        private void ImagesAndVideo_Checked(object sender, RoutedEventArgs e)
        {
            this.contentTypeFilder = PhotoImportContentTypeFilter.ImagesAndVideos;

            // enable find since the filter has changed
            this.findAllItemsButton.IsEnabled = true;
            this.findNewItemsButton.IsEnabled = true;
        }

        private void NoSubFolder_checked(object sender, RoutedEventArgs e)
        {
            this.session.SubfolderCreationMode = PhotoImportSubfolderCreationMode.DoNotCreateSubfolders;
        }

        private void FileDateSubfolder_checked(object sender, RoutedEventArgs e)
        {
            this.session.SubfolderCreationMode = PhotoImportSubfolderCreationMode.CreateSubfoldersFromFileDate;
        }

        private void ExifDateSubfolder_checked(object sender, RoutedEventArgs e)
        {
            this.session.SubfolderCreationMode = PhotoImportSubfolderCreationMode.CreateSubfoldersFromExifDate;
        }

        private void KeepOriginalFolder_checked(object sender, RoutedEventArgs e)
        {
            this.session.SubfolderCreationMode = PhotoImportSubfolderCreationMode.KeepOriginalFolderStructure;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string propName)
        {
            PropertyChanged(this, new PropertyChangedEventArgs(propName));
        }
    }
}

                
                