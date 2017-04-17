using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI.Xaml.Data;

namespace DataVirtualizationSample
{
    // EventArgs class for the CacheChanged event 
    public class CacheChangedEventArgs<T> : EventArgs
    {
        public T oldItem { get; set; }
        public T newItem { get; set; }
        public int itemIndex { get; set; }
    }

    // Implements a relatively simple cache for items based on a set of ranges
    class ItemCacheManager<T>
    {
        // data structure to hold all the items that are in the ranges the cache manager is looking after
        private List<CacheEntryBlock<T>> cacheBlocks;

        // List of ranges for items that are not present in the cache
        internal ItemIndexRangeList requests;

        // list of ranges for items that are present in the cache
        private ItemIndexRangeList cachedResults;
        // Range of items that is currently being requested
        private ItemIndexRange requestInProgress;
        // Used to be able to cancel outstanding requests
        private CancellationTokenSource cancelTokenSource;
        // Callback that will be used to request data
        private fetchDataCallbackHandler fetchDataCallback;
        // Maximum number of items that can be fetched in one batch
        private int maxBatchFetchSize;
        // Timer to optimize the the fetching of data so we throttle requests if the list is still changing
        private Windows.UI.Xaml.DispatcherTimer timer;

#if DEBUG
        // Name for trace messages, and when debugging so you know which instance of the cache manager you are dealing with
        string debugName = string.Empty;
#endif
        public ItemCacheManager(fetchDataCallbackHandler callback, int batchsize = 50, string debugName = "ItemCacheManager")
        {
            cacheBlocks = new List<CacheEntryBlock<T>>();
            requests = new ItemIndexRangeList();
            cachedResults = new ItemIndexRangeList();
            fetchDataCallback = callback;
            maxBatchFetchSize = batchsize;
            //set up a timer that is used to delay fetching data so that we can catch up if the list is scrolling fast
            timer = new Windows.UI.Xaml.DispatcherTimer();
            timer.Tick += (sender, args) =>
            {
                fetchData();
            };
            timer.Interval = new TimeSpan(20 * 10000);

#if DEBUG
            this.debugName = debugName;
#endif
#if TRACE_DATASOURCE
            Debug.WriteLine(debugName + "* Cache initialized/reset");
#endif
        }

        public event TypedEventHandler<object, CacheChangedEventArgs<T>> CacheChanged;

        /// <summary>
        /// Indexer for access to the item cache
        /// </summary>
        /// <param name="index">Item Index</param>
        /// <returns></returns>
        public T this[int index]
        {
            get
            {
                // iterates through the cache blocks to find the item
                foreach (CacheEntryBlock<T> block in cacheBlocks)
                {
                    if (index >= block.FirstIndex && index <= block.lastIndex)
                    {
                        return block.Items[index - block.FirstIndex];
                    }
                }
                return default(T);
            }
            set
            {
                // iterates through the cache blocks to find the right block
                for (int i = 0; i < cacheBlocks.Count; i++)
                {
                    CacheEntryBlock<T> block = cacheBlocks[i];
                    if (index >= block.FirstIndex && index <= block.lastIndex)
                    {
                        block.Items[index - block.FirstIndex] = value;
                        //register that we have the result in the cache
                        if (value != null) { cachedResults.Add((uint)index, 1); }
                        return;
                    }
                    // We have moved past the block where the item is supposed to live
                    if (block.FirstIndex > index)
                    {
                        AddOrExtendBlock(index, value, i);
                        return;
                    }
                }
                // No blocks exist, so creating a new block
                AddOrExtendBlock(index, value, cacheBlocks.Count);

            }
        }

        // Extends an existing block if the item fits at the end, or creates a new block
        private void AddOrExtendBlock(int index, T value, int insertBeforeBlock)
        {
            if (insertBeforeBlock > 0)
            {
                CacheEntryBlock<T> block = cacheBlocks[insertBeforeBlock - 1];
                if (block.lastIndex == index - 1)
                {
                    T[] newItems = new T[block.Length + 1];
                    Array.Copy(block.Items, newItems, (int)block.Length);
                    newItems[block.Length] = value;
                    block.Length++;
                    block.Items = newItems;
                    return;
                }
            }
            CacheEntryBlock<T> newBlock = new CacheEntryBlock<T>() { FirstIndex = index, Length = 1, Items = new T[] { value } };
            cacheBlocks.Insert(insertBeforeBlock, newBlock);
        }


        /// <summary>
        /// Updates the desired item range of the cache, discarding items that are not needed, and figuring out which items need to be requested. It will then kick off a fetch if required.
        /// </summary>
        /// <param name="ranges">New set of ranges the cache should hold</param>
        public void UpdateRanges(ItemIndexRange[] ranges)
        {
            //Normalize ranges to get a unique set of discontinuous ranges
            ranges = NormalizeRanges(ranges);

            // Fail fast if the ranges haven't changed
            if (!HasRangesChanged(ranges)) { return; }

            //To make the cache update easier, we'll create a new set of CacheEntryBlocks
            List<CacheEntryBlock<T>> newCacheBlocks = new List<CacheEntryBlock<T>>();
            foreach (ItemIndexRange range in ranges)
            {
                CacheEntryBlock<T> newBlock = new CacheEntryBlock<T>() { FirstIndex = range.FirstIndex, Length = range.Length, Items = new T[range.Length] };
                newCacheBlocks.Add(newBlock);
            }

#if TRACE_DATASOURCE
            string s = "┌ " + debugName + ".UpdateRanges: ";
            foreach (ItemIndexRange range in ranges)
            {
                s += range.FirstIndex + "->" + range.LastIndex + " ";
            }
            Debug.WriteLine(s);
#endif
            //Copy over data to the new cache blocks from the old ones where there is overlap
            int lastTransferred = 0;
            for (int i = 0; i < ranges.Length; i++)
            {
                CacheEntryBlock<T> newBlock = newCacheBlocks[i];
                ItemIndexRange range = ranges[i];
                int j = lastTransferred;
                while (j < this.cacheBlocks.Count && this.cacheBlocks[j].FirstIndex <= ranges[i].LastIndex)
                {
                    ItemIndexRange overlap, oldEntryRange;
                    ItemIndexRange[] added, removed;
                    CacheEntryBlock<T> oldBlock = this.cacheBlocks[j];
                    oldEntryRange = new ItemIndexRange(oldBlock.FirstIndex, oldBlock.Length);
                    bool hasOverlap = oldEntryRange.DiffRanges(range, out overlap, out removed, out added);
                    if (hasOverlap)
                    {
                        Array.Copy(oldBlock.Items, overlap.FirstIndex - oldBlock.FirstIndex, newBlock.Items, overlap.FirstIndex - range.FirstIndex, (int)overlap.Length);
#if TRACE_DATASOURCE
                        Debug.WriteLine("│ Transfering cache items " + overlap.FirstIndex + "->" + overlap.LastIndex);
#endif
                    }
                    j++;
                    if (ranges.Length > i + 1 && oldBlock.lastIndex < ranges[i + 1].FirstIndex) { lastTransferred = j; }
                }
            }
            //swap over to the new cache
            this.cacheBlocks = newCacheBlocks;

            //figure out what items need to be fetched because we don't have them in the cache
            this.requests = new ItemIndexRangeList(ranges);
            ItemIndexRangeList newCachedResults = new ItemIndexRangeList();

            // Use the previous knowlege of what we have cached to form the new list
            foreach (ItemIndexRange range in ranges)
            {
                foreach (ItemIndexRange cached in this.cachedResults)
                {
                    ItemIndexRange overlap;
                    ItemIndexRange[] added, removed;
                    bool hasOverlap = cached.DiffRanges(range, out overlap, out removed, out added);
                    if (hasOverlap) { newCachedResults.Add(overlap); }
                }
            }
            // remove the data we know we have cached from the results
            foreach (ItemIndexRange range in newCachedResults)
            {
                this.requests.Subtract(range);
            }
            this.cachedResults = newCachedResults;

            startFetchData();

#if TRACE_DATASOURCE
            s = "└ Pending requests: ";
            foreach (ItemIndexRange range in this.requests)
            {
                s += range.FirstIndex + "->" + range.LastIndex + " ";
            }
            Debug.WriteLine(s);
#endif 
        }

        // Compares the new ranges against the previous ones to see if they have changed
        private bool HasRangesChanged(ItemIndexRange[] ranges)
        {
            if (ranges.Length != cacheBlocks.Count)
            {
                return true;
            }
            for (int i = 0; i < ranges.Length; i++)
            {
                ItemIndexRange r = ranges[i];
                CacheEntryBlock<T> block = this.cacheBlocks[i];
                if (r.FirstIndex != block.FirstIndex || r.LastIndex != block.lastIndex)
                {
                    return true;
                }
            }
            return false;
        }

        // Gets the first block of items that we don't have values for
        public ItemIndexRange GetFirstRequestBlock(int maxsize = 50)
        {
            if (this.requests.Count > 0)
            {
                ItemIndexRange range = this.requests[0];
                if (range.Length > 50) range = new ItemIndexRange(range.FirstIndex, 50);
                return range;
            }
            return null;
        }


        // Throttling function for fetching data. Forces a wait of 20ms before making the request.
        // If another fetch is requested in that time, it will reset the timer, so we don't fetch data if the view is actively scrolling
        public void startFetchData()
        {
            // Verify if an active request is still needed
            if (this.requestInProgress != null)
            {
                if (this.requests.Intersects(requestInProgress))
                {
                    return;
                }
                else
                {
                    //cancel the existing request
#if TRACE_DATASOURCE
                    Debug.WriteLine("> " + debugName + " Cancelling request: " + requestInProgress.FirstIndex + "->" + requestInProgress.LastIndex);
#endif
                    cancelTokenSource.Cancel();
                }
            }

            //Using a timer to delay fetching data by 20ms, if another range comes in that time, then the timer is reset.
            timer.Stop();
            timer.Start();
        }

        public delegate Task<T[]> fetchDataCallbackHandler(ItemIndexRange range, CancellationToken ct);

        // Called by the timer to make a request for data
        public async void fetchData()
        {
            //Stop the timer so we don't get fired again unless data is requested
            timer.Stop();
            if (this.requestInProgress != null)
            {
                // Verify if an active request is still needed
                if (this.requests.Intersects(requestInProgress))
                {
                    return;
                }
                else
                {
                    // Cancel the existing request
                    Debug.WriteLine(">" + debugName + " Cancelling request: " + requestInProgress.FirstIndex + "->" + requestInProgress.LastIndex);
                    cancelTokenSource.Cancel();
                }
            }

            ItemIndexRange nextRequest = GetFirstRequestBlock(maxBatchFetchSize);
            if (nextRequest != null)
            {
                cancelTokenSource = new CancellationTokenSource();
                CancellationToken ct = cancelTokenSource.Token;
                requestInProgress = nextRequest;
                T[] data = null;
                try
                {
#if TRACE_DATASOURCE
                    Debug.WriteLine(">" + debugName + " Fetching items " + nextRequest.FirstIndex + "->" + nextRequest.LastIndex);
#endif
                    // Use the callback to get the data, passing in a cancellation token
                    data = await fetchDataCallback(nextRequest, ct);

                    if (!ct.IsCancellationRequested)
                    {
#if TRACE_DATASOURCE
                        Debug.WriteLine(">" + debugName + " Inserting items into cache at: " + nextRequest.FirstIndex + "->" + (nextRequest.FirstIndex + data.Length - 1));
#endif
                        for (int i = 0; i < data.Length; i++)
                        {
                            int cacheIndex = (int)(nextRequest.FirstIndex + i);

                            T oldItem = this[cacheIndex];
                            T newItem = data[i];

                            if (!newItem.Equals(oldItem))
                            {
                                this[cacheIndex] = newItem;

                                // Fire CacheChanged so that the datasource can fire its INCC event, and do other work based on the item having data
                                if (CacheChanged != null)
                                {
                                    CacheChanged(this, new CacheChangedEventArgs<T>() { oldItem = oldItem, newItem = newItem, itemIndex = cacheIndex });
                                }
                            }
                        }
                        requests.Subtract(new ItemIndexRange(nextRequest.FirstIndex, (uint)data.Length));
                    }
                }
                // Try/Catch is needed as cancellation is via an exception
                catch (OperationCanceledException) { }
                finally
                {
                    requestInProgress = null;
                    // Start another request if required
                    fetchData();
                }
            }
        }


        /// <summary>
        /// Merges a set of ranges to form a new set of non-contiguous ranges
        /// </summary>
        /// <param name="ranges">The list of ranges to merge</param>
        /// <returns>A smaller set of merged ranges</returns>
        private ItemIndexRange[] NormalizeRanges(ItemIndexRange[] ranges)
        {
            List<ItemIndexRange> results = new List<ItemIndexRange>();
            foreach (ItemIndexRange range in ranges)
            {
                bool handled = false;
                for (int i = 0; i < results.Count; i++)
                {
                    ItemIndexRange existing = results[i];
                    if (range.ContiguousOrOverlaps(existing))
                    {
                        results[i] = existing.Combine(range);
                        handled = true;
                        break;
                    }
                    else if (range.FirstIndex < existing.FirstIndex)
                    {
                        results.Insert(i, range);
                        handled = true;
                        break;
                    }
                }
                if (!handled) { results.Add(range); }
            }
            return results.ToArray();
        }


        // Sees if the value is in our cache if so it returns the index
        public int IndexOf(T value)
        {
            foreach (CacheEntryBlock<T> entry in cacheBlocks)
            {
                int index = Array.IndexOf<T>(entry.Items, value);
                if (index != -1) return index + entry.FirstIndex;
            }
            return -1;
        }

        // Type for the cache blocks
        class CacheEntryBlock<ITEMTYPE>
        {
            public int FirstIndex;
            public uint Length;
            public ITEMTYPE[] Items;

            public int lastIndex { get { return FirstIndex + (int)Length - 1; } }
        }
    }
}
