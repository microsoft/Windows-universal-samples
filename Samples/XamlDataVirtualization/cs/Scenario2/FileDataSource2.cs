using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Search;
using Windows.UI.Core;
using Windows.UI.Xaml.Data;

namespace DataVirtualizationSample
{
    //********************************************************************************************
    //*
    //* Note: This sample uses a custom compiler constant to enable tracing. If you add
    //* TRACE_DATASOURCE to the Conditional compilation symbols of the Build tab of the
    //* Project Properties window, then the application will spit out trace data to the
    //* Output window while debugging.
    //*
    //********************************************************************************************

    /// <summary>
    /// A custom datasource over the file system that supports data virtualization
    /// </summary>
    class FileDataSource2 : INotifyCollectionChanged, System.Collections.IList, IItemsRangeInfo, ISelectionInfo
    {
        // Folder that we are browsing
        private StorageFolder _folder;
        // Query object that will tell us if the folder content changed
        private StorageFileQueryResult _queryResult;
        // Dispatcher so we can marshal calls back to the UI thread
        private CoreDispatcher _dispatcher;

        // Cache for the file data that is currently being used
        private ItemCacheManager<FileItem> itemCache;

        // Total number of files available
        private int _count = 1;

        public event NotifyCollectionChangedEventHandler CollectionChanged;

        private FileDataSource2()
        {
            //Setup the dispatcher for the UI thread
            _dispatcher = Windows.UI.Xaml.Window.Current.Dispatcher;

            // The ItemCacheManager does most of the heavy lifting. We pass it a callback that it will use to actually fetch data, and the max size of a request
            this.itemCache = new ItemCacheManager<FileItem>(fetchDataCallback, 50);
            this.itemCache.CacheChanged += ItemCache_CacheChanged;

            // Create another ItemCacheManager for the keys for selection
            this.selectionCache = new ItemCacheManager<string>(fetchSelectionDataCallback, 50, "selectionCache");
        }

        // Handles a change notification for the list of files from the OS
        public async void ResetCollection()
        {
            // Update the selection cache to match the filesystem after the updates
            await RemapSelection();

            // Unhook the old change notification
            if (itemCache != null)
            {
                this.itemCache.CacheChanged -= ItemCache_CacheChanged;
            }

            // Create a new instance of the cache manager
            this.itemCache = new ItemCacheManager<FileItem>(fetchDataCallback, 50);
            this.itemCache.CacheChanged += ItemCache_CacheChanged;
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
        }


        //
        // Implementation of ISelectionInfo
        //

        // Storage for the keys of the selected items
        private ItemCacheManager<string> selectionCache;
        // List of ranges that form the selection
        private ItemIndexRangeList selection = new ItemIndexRangeList();

        // Called when an item (or items) are selected
        public void SelectRange(ItemIndexRange range)
        {
            selection.Add(range);
            selectionCache.UpdateRanges(selection.ToArray());
        }

        // Called when an item (or items) are deselected
        public void DeselectRange(ItemIndexRange range)
        {
            selection.Subtract(range);
            selectionCache.UpdateRanges(selection.ToArray());
        }

        //  Called to determine if an item is selected 
        public bool IsSelected(int index)
        {
            foreach (ItemIndexRange range in selection)
            {
                if (index >= range.FirstIndex && index <= range.LastIndex) return true;
            }
            return false;
        }

        // Called to get the selected ranges 
        public IReadOnlyList<ItemIndexRange> GetSelectedRanges()
        {
            return selection.ToList();
        }

        // Callback from the selection cache manager
        // Retrieves the keys for selected items
        private async Task<string[]> fetchSelectionDataCallback(ItemIndexRange batch, CancellationToken ct)
        {
#if TRACE_DATASOURCE
            Debug.WriteLine("# SelectionDataCallback: " + batch.FirstIndex + "->" + batch.LastIndex);
#endif
            // See if we already have the item in the data cache, if so get the key from there so we don't need to go to the filesystem
            var file = itemCache[batch.FirstIndex];
            if (file != null)
            {
                return new string[] { file.Key };
            }

            // Go get the keys from the file system if necessary
            IReadOnlyList<StorageFile> results = await _queryResult.GetFilesAsync((uint)batch.FirstIndex, batch.Length).AsTask(ct);
            List<string> keys = new List<string>();
            if (results != null)
            {
                for (int i = 0; i < results.Count; i++)
                {
                    ct.ThrowIfCancellationRequested();
                    keys.Add(results[i].FolderRelativeId);
                }
            }
            return keys.ToArray();
        }

        // Called after a reset to figure out the new indexes for the selected items
        private async Task RemapSelection()
        {
            ItemIndexRangeList oldSelection = selection;
            ItemCacheManager<string> oldSelectionCache = selectionCache;
            ItemIndexRangeList newSelection = new ItemIndexRangeList();
            ItemCacheManager<string> newSelectionCache = new ItemCacheManager<string>(fetchSelectionDataCallback, 50, "newSelectionCache");

            foreach (ItemIndexRange r in oldSelection)
            {
                IReadOnlyList<StorageFile> results = null;
                int lastResultOffset = 0, lastResultsItemIndex = 0;
                for (int i = 0; i < r.Length; i++)
                {
                    int origIndex = r.FirstIndex + i;
                    string fileid = oldSelectionCache[origIndex];
                    bool matched = false;

                    // Optimization to be able to work in batches. Once we get a batch of files from the filesystem we use that to hunt
                    // for matches rather than having to go ask for the index of each file
                    if (results != null)
                    {
                        for (int j = lastResultOffset + 1; j < results.Count; j++)
                        {
                            if (results[j].FolderRelativeId == fileid)
                            {
                                lastResultOffset = j;
                                int itemIndex = lastResultsItemIndex + j;
                                newSelection.Add((uint)itemIndex, 1);
                                newSelectionCache[itemIndex] = oldSelectionCache[origIndex];
                                matched = true;
                                break;
                            }
                        }
                    }
                    if (!matched)
                    {
                        // Get a starting point for the index of the file
                        lastResultsItemIndex = (int)(await _queryResult.FindStartIndexAsync(fileid.Substring(fileid.LastIndexOf('\\') + 1)));
                        lastResultOffset = 0;
                        // Get the files at that point and see if the keys actually match
                        results = await _queryResult.GetFilesAsync((uint)lastResultsItemIndex, 50);
                        if (results[lastResultOffset].FolderRelativeId == fileid)
                        {
                            newSelection.Add((uint)lastResultsItemIndex, 1);
                            newSelectionCache[lastResultsItemIndex] = oldSelectionCache[origIndex];
                        }
                        else
                        {
                            // We can't find the item, so its no longer part of the selection
                        }
                    }
                }
            }
            // Update the fields for the new selection
            selection = newSelection;
            selectionCache = newSelectionCache;
        }

        #region Filesystem specific implementation details

        // Factory method to create the datasource
        // Requires async work which is why it needs a factory rather than being part of the constructor
        public static async Task<FileDataSource2> GetDataSoure(string path)
        {
            FileDataSource2 ds = new FileDataSource2();
            StorageFolder f = await StorageFolder.GetFolderFromPathAsync(path);
            await ds.SetFolder(f);
            return ds;
        }

        // Set functionality for the folder
        public async Task SetFolder(StorageFolder folder)
        {
            //unhook the old contents changed event if applicable
            if (_queryResult != null)
            {
                _queryResult.ContentsChanged -= QueryResult_ContentsChanged;
            }
            // Initialize the query and register for changes
            _folder = folder;
            QueryOptions options = new QueryOptions();
            // options.IndexerOption = IndexerOption.DoNotUseIndexer;
            _queryResult = _folder.CreateFileQueryWithOptions(options);
            _queryResult.ContentsChanged += QueryResult_ContentsChanged;
            await UpdateCount();
        }

        // Handler for when the filesystem notifies us of a change to the file list
        private void QueryResult_ContentsChanged(IStorageQueryResultBase sender, object args)
        {
            // This callback can occur on a different thread so we need to marshal it back to the UI thread
            if (!_dispatcher.HasThreadAccess)
            {
                var t = _dispatcher.RunAsync(CoreDispatcherPriority.Normal, ResetCollection);
            }
            else
            {
                ResetCollection();
            }
        }
        async Task UpdateCount()
        {
            _count = (int)await _queryResult.GetItemCountAsync();
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
        }

        //Event fired when items are inserted in the cache
        //Used to fire our collection changed event
        private void ItemCache_CacheChanged(object sender, CacheChangedEventArgs<FileItem> args)
        {
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, args.oldItem, args.newItem, args.itemIndex));
            }
        }
        #endregion

        #region IList Implementation

        public bool Contains(object value)
        {
            return IndexOf(value) != -1;
        }

        public int IndexOf(object value)
        {
            return (value != null) ? itemCache.IndexOf((FileItem)value) : -1;
        }

        public object this[int index]
        {
            get
            {
                // The cache will return null if it doesn't have the item. Once the item is fetched it will fire a changed event so that we can inform the list control
                return itemCache[index];
            }
            set
            {
                throw new NotImplementedException();
            }
        }
        public int Count
        {
            get { return _count; }
        }

        #endregion

        #region IItemsRangeInfo Implementation
        //Required for the IItemsRangeInfo interface
        public void Dispose()
        {
            itemCache = null;
        }

        /// <summary>
        /// Primary method for IItemsRangeInfo interface
        /// Is called when the list control's view is changed
        /// </summary>
        /// <param name="visibleRange">The range of items that are actually visible</param>
        /// <param name="trackedItems">Additional set of ranges that the list is using, for example the buffer regions and focussed element</param>
        public void RangesChanged(ItemIndexRange visibleRange, IReadOnlyList<ItemIndexRange> trackedItems)
        {
#if TRACE_DATASOURCE
            string s = string.Format("* RangesChanged fired: Visible {0}->{1}", visibleRange.FirstIndex, visibleRange.LastIndex);
            foreach (ItemIndexRange r in trackedItems) { s += string.Format(" {0}->{1}", r.FirstIndex, r.LastIndex); }
            Debug.WriteLine(s);
#endif
            // We know that the visible range is included in the broader range so don't need to hand it to the UpdateRanges call
            // Update the cache of items based on the new set of ranges. It will callback for additional data if required
            itemCache.UpdateRanges(trackedItems.ToArray());
        }


        // Callback from itemcache that it needs items to be retrieved
        private async Task<FileItem[]> fetchDataCallback(ItemIndexRange batch, CancellationToken ct)
        {
            // Fetch file objects from filesystem
            IReadOnlyList<StorageFile> results = await _queryResult.GetFilesAsync((uint)batch.FirstIndex, Math.Max(batch.Length, 20)).AsTask(ct);
            List<FileItem> files = new List<FileItem>();
            if (results != null)
            {
                for (int i = 0; i < results.Count; i++)
                {
                    ct.ThrowIfCancellationRequested();
                    // Create our FileItem object with the file data and thumbnail 
                    FileItem newItem = await FileItem.fromStorageFile(results[i], ct);
                    files.Add(newItem);
                }
            }
            return files.ToArray();
        }

        #endregion

        #region Parts of IList Not Implemented

        public int Add(object value)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, object value)
        {
            throw new NotImplementedException();
        }

        public bool IsFixedSize
        {
            get { return false; }
        }

        public bool IsReadOnly
        {
            get { return false; }
        }

        public void Remove(object value)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }
        public void CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        public bool IsSynchronized
        {
            get { throw new NotImplementedException(); }
        }

        public object SyncRoot
        {
            get { throw new NotImplementedException(); }
        }

        public System.Collections.IEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion

    }

}
