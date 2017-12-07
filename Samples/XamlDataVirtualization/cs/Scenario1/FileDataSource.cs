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
    class FileDataSource : INotifyCollectionChanged, System.Collections.IList, IItemsRangeInfo
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

        private FileDataSource()
        {
            //Setup the dispatcher for the UI thread
            _dispatcher = Windows.UI.Xaml.Window.Current.Dispatcher;

            // The ItemCacheManager does most of the heavy lifting. We pass it a callback that it will use to actually fetch data, and the max size of a request
            this.itemCache = new ItemCacheManager<FileItem>(fetchDataCallback, 50);
            this.itemCache.CacheChanged += ItemCache_CacheChanged;
        }

        // Factory method to create the datasource
        // Requires async work which is why it needs a factory rather than being part of the constructor
        public static async Task<FileDataSource> GetDataSoure(string path)
        {
            FileDataSource ds = new FileDataSource();
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

        // Handles a change notification for the list of files from the OS
        private void ResetCollection()
        {
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

        async Task UpdateCount()
        {
            _count = (int)await _queryResult.GetItemCountAsync();
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
        }

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
        // Using this callback model abstracts the details of this specific datasource from the cache implementation
        private async Task<FileItem[]> fetchDataCallback(ItemIndexRange batch, CancellationToken ct)
        {
            // Fetch file objects from filesystem
            IReadOnlyList<StorageFile> results = await _queryResult.GetFilesAsync((uint)batch.FirstIndex, Math.Max(batch.Length, 20)).AsTask(ct);
            List<FileItem> files = new List<FileItem>();
            if (results != null)
            {
                for (int i = 0; i < results.Count; i++)
                {
                    // Check if request has been cancelled, if so abort getting additional data
                    ct.ThrowIfCancellationRequested();
                    // Create our FileItem object with the file data and thumbnail 
                    FileItem newItem = await FileItem.fromStorageFile(results[i], ct);
                    files.Add(newItem);
                }
            }
            return files.ToArray();
        }

        // Event fired when items are inserted in the cache
        // Used to fire our collection changed event
        private void ItemCache_CacheChanged(object sender, CacheChangedEventArgs<FileItem> args)
        {
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, args.oldItem, args.newItem, args.itemIndex));
            }
        }

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
