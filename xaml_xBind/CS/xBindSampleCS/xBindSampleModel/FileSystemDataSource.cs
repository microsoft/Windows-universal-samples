using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Search;
using Windows.UI.Core;
using Windows.UI.Xaml.Interop;
using Windows.UI.Xaml.Media.Imaging;

namespace xBindSampleModel
{

    public sealed class FileItem
    {
        public string DisplayName { get; set; }
        public string Path { get; set; }
        public int FileSize { get; set; }
        public DateTimeOffset Date { get; set; }
        public int ImageWidth { get; private set; }
        public int ImageHeight { get; private set; }

        private BitmapImage _imageData;
        public BitmapImage ImageData { get { return _imageData; } }

        private async Task FetchImageAsync()
        {
            _imageData = new BitmapImage();
            StorageFile f = await StorageFile.GetFileFromPathAsync(Path);
            await _imageData.SetSourceAsync(await f.GetThumbnailAsync(ThumbnailMode.SingleItem));
        }
        public string Key { get; private set; }

        internal async static Task<FileItem> fromStorageFile(StorageFile f)
        {
            FileItem item = new FileItem();
            item.Path = f.Path;
            item.DisplayName = f.DisplayName;
            BasicProperties bp = await f.GetBasicPropertiesAsync();
            item.FileSize = (int)bp.Size;
            item.Date = bp.DateModified;
            item.Key = f.FolderRelativeId;
            ImageProperties ip = await f.Properties.GetImagePropertiesAsync();
            item.ImageWidth = (int)ip.Width;
            item.ImageHeight = (int)ip.Height;
            var t = item.FetchImageAsync();
            return item;
        }

        public string prettyDate
        {
            get
            {
                DateTime now = DateTime.Now;
                if (Date.Date == now.Date)
                {
                    return Date.ToLocalTime().ToString("T");
                }
                else if (Date.Date == now.Date.AddDays(-1))
                {
                    return "Yesterday";
                }
                else return Date.ToLocalTime().ToString("d MMM yyyy");
            }
        }

        public string prettyFileSize
        {
            get { return FileSize.ToString("N0") + " bytes"; }
        }

        public string prettyImageSize
        {
            get { return ImageWidth + " x " + ImageHeight + " px"; }
        }
    }


    public sealed class FileDataSource : INotifyCollectionChanged, System.Collections.IList, IObservableVector<FileItem>
    {
        private StorageFileQueryResult _queryResult;
        private CoreDispatcher dispatcher;
        private int _count = 0;
        private List<FileItem> _cache;

        public event NotifyCollectionChangedEventHandler CollectionChanged;
        public event VectorChangedEventHandler<FileItem> VectorChanged;

        public FileDataSource()
        {
            dispatcher = Windows.UI.Xaml.Window.Current.Dispatcher;
            _cache = new List<FileItem>();
        }

        //WinRT wrapper
        public IAsyncAction SetupDataSource(StorageFolder folder)
        {
            return this.SetupDataSourceImpl(folder).AsAsyncAction();
        }

        private async Task SetupDataSourceImpl(StorageFolder folder)
        {
            this.Folder = folder;
            QueryOptions options = new QueryOptions();
            options.IndexerOption = IndexerOption.UseIndexerWhenAvailable;
            _queryResult = this.Folder.CreateFileQueryWithOptions(options);
            _queryResult.ContentsChanged += QueryResult_ContentsChanged;
            await FetchData();
        }

        public IAsyncAction SetupDataSourceUsingPicturesFolder()
        {
            return SetupDataSourceUsingPicturesFolderImpl().AsAsyncAction();
        }
        private async Task SetupDataSourceUsingPicturesFolderImpl()
        {
            StorageLibrary pictures = await Windows.Storage.StorageLibrary.GetLibraryAsync(Windows.Storage.KnownLibraryId.Pictures);
            await SetupDataSourceImpl(pictures.SaveFolder);
        }
        public StorageFolder Folder { get; set; }

        private void QueryResult_ContentsChanged(IStorageQueryResultBase sender, object args)
        {
            if (!dispatcher.HasThreadAccess)
            {
                var t = dispatcher.RunAsync(CoreDispatcherPriority.Normal, ResetCollection);
            }
        }

        public void ResetCollection()
        {
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
            if (VectorChanged != null)
            {
                VectorChanged(this, new VectorChangedEventArgs() { CollectionChange = CollectionChange.Reset });
            }
        }

        private const int BATCHSIZE = 50;

        async Task FetchData()
        {
            _count = (int)await _queryResult.GetItemCountAsync();
            this._cache.Clear();
            this._cache.AddRange(new FileItem[_count]);
            for (int i = 0; i < _count; i += BATCHSIZE)
            {
                await fetchBatch(i);
            }

            if (VectorChanged != null)
            {
                VectorChanged(this, new VectorChangedEventArgs() { CollectionChange = CollectionChange.Reset });
            }
            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
        }

        async Task fetchBatch(int index)
        {
            IReadOnlyList<StorageFile> results = await _queryResult.GetFilesAsync((uint)index, (uint)BATCHSIZE);
            List<Task> tasks = new List<Task>(BATCHSIZE);
            for (int i = 0; i < results.Count; i++)
            {
                tasks.Add(
                    FileItem.fromStorageFile(results[i]).ContinueWith((f, j) =>
                    {
                        int cacheIndex = index + (int)j;
                        _cache[cacheIndex] = f.Result;
                    }, i));
            }
            await Task.WhenAll(tasks.ToArray());
        }

        bool IList.Contains(object item)
        {
            return ((IList)this).IndexOf(item) != -1;
        }

        object this[int index]
        {
            get
            {
                object result = null;
                if (index < _cache.Count) result = _cache[index];
                return result;
            }
            set
            {
                FileItem f = value as FileItem;
                if (f != null)
                    _cache[index] = f;
                else
                    throw new ArgumentException("The value is expected to be a fileitem");
            }
        }
        public int Count
        {
            get { return _count; }
        }

        bool IList.IsFixedSize { get { return false; } }

        bool IList.IsReadOnly { get { return true; } }


        int ICollection.Count { get { return _count; } }

        bool ICollection.IsSynchronized { get { throw new NotImplementedException(); } }

        object ICollection.SyncRoot { get { throw new NotImplementedException(); } }

        object IList.this[int index]
        {
            get
            {
                return this[index];
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        int IList.Add(object item)
        {
            throw new NotImplementedException();
        }

        void IList.Clear()
        {
            throw new NotImplementedException();
        }

        int IList.IndexOf(object item)
        {
            return _cache.IndexOf((FileItem)item);
        }

        void IList.Insert(int index, object value)
        {
            throw new NotImplementedException();
        }

        void IList.Remove(object value)
        {
            throw new NotImplementedException();
        }

        void IList.RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        void ICollection.CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #region IObservableVector Implementation
        FileItem IList<FileItem>.this[int index]
        {
            get
            {
                FileItem result = null;
                if (index < _cache.Count) result = _cache[index];
                return result;
            }

            set
            {
                _cache[index] = value;
            }
        }

        int ICollection<FileItem>.Count
        {
            get
            {
                return _count;
            }
        }

        bool ICollection<FileItem>.IsReadOnly
        {
            get
            {
                return true;
            }
        }
  
        void ICollection<FileItem>.Add(FileItem item)
        {
            throw new NotImplementedException();
        }

        void ICollection<FileItem>.Clear()
        {
            throw new NotImplementedException();
        }

        bool ICollection<FileItem>.Contains(FileItem item)
        {
            throw new NotImplementedException();
        }

        void ICollection<FileItem>.CopyTo(FileItem[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        IEnumerator<FileItem> IEnumerable<FileItem>.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        int IList<FileItem>.IndexOf(FileItem item)
        {
            throw new NotImplementedException();
        }

        void IList<FileItem>.Insert(int index, FileItem item)
        {
            throw new NotImplementedException();
        }

        bool ICollection<FileItem>.Remove(FileItem item)
        {
            throw new NotImplementedException();
        }

        void IList<FileItem>.RemoveAt(int index)
        {
            throw new NotImplementedException();
        }
        #endregion
    }

}


