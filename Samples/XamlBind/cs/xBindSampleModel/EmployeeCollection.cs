using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using Windows.Foundation.Collections;

namespace xBindSampleModel
{
    public sealed class VectorChangedEventArgs : IVectorChangedEventArgs
    {
        public CollectionChange CollectionChange { get; set; }
        public uint Index { get; set; }
    }


    //Implements both IObservableVector and INotifyCollectionChanged and as its used from both C++ and C# apps
    public sealed class EmployeeCollection : Windows.Foundation.Collections.IObservableVector<IEmployee>, IList<IEmployee>, INotifyCollectionChanged, IList
    {
        private ObservableCollection<IEmployee> data;
        public event VectorChangedEventHandler<IEmployee> VectorChanged;
        public event NotifyCollectionChangedEventHandler CollectionChanged;

        public EmployeeCollection()
        {
            data = new ObservableCollection<IEmployee>();
            data.CollectionChanged += Data_CollectionChanged;
        }

        private void Data_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (VectorChanged != null)
            {
                VectorChangedEventArgs args = new VectorChangedEventArgs();
                args.CollectionChange = CollectionChange.Reset;
                VectorChanged(this, args);
            }

            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                CollectionChanged(this, e);
            }
        }

        #region IObservableVector
        //IObservableVector derives from IList<T> so most of the interface members are strongly typed

        public IEmployee this[int index]
        {
            get { return data[index]; }
            set { data[index] = value; }
        }

        public int Count { get { return data.Count; } }
        public bool IsReadOnly { get { return false; } }

        public void Add(IEmployee item)
        {
            data.Add(item);
        }

        public void Clear()
        {
            data.Clear();
        }
        public bool Contains(IEmployee item)
        {
            return data.Contains(item);
        }

        public void CopyTo(IEmployee[] array, int arrayIndex)
        {
            data.CopyTo(array, arrayIndex);
        }
        public IEnumerator<IEmployee> GetEnumerator() { return data.GetEnumerator(); }

        public int IndexOf(IEmployee item) { return data.IndexOf(item); }

        public void Insert(int index, IEmployee item)
        {
            data.Insert(index, item);
        }

        public bool Remove(IEmployee item)
        {
            return data.Remove(item);
        }

        public void RemoveAt(int index)
        {
            data.RemoveAt(index);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return data.GetEnumerator();
        }
        #endregion

        #region INotifyCollectionChanged
        //INotifyCollection changed inherits from IList (non generic) so most of its members are not strongly typed

        object IList.this[int index]
        {
            get { return data[index]; }
            set { data[index] = (Employee)value; }
        }

        int ICollection.Count { get { return data.Count; } }
        bool IList.IsFixedSize { get { return false; } }
        bool ICollection.IsSynchronized { get { return false; } }
        object ICollection.SyncRoot { get { throw new NotImplementedException(); } }

        int IList.Add(object item)
        {
            data.Add((IEmployee)item);
            return data.Count;
        }

        bool IList.IsReadOnly { get { return false; } }

        bool IList.Contains(object item) { return data.Contains(item); }

        int IList.IndexOf(object item) { return data.IndexOf((IEmployee)item); }

        void IList.Insert(int index, object item)
        {
            data.Insert(index, (IEmployee)item);
        }

        void IList.Remove(object item)
        {
            data.Remove((IEmployee)item);
        }

        void IList.Clear()
        {
            data.Clear();
        }

        void IList.RemoveAt(int index)
        {
            data.RemoveAt(index);
        }

        void ICollection.CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }
        #endregion
    }

    internal class MapChangedEventArgs<K> : IMapChangedEventArgs<K>
    {
        public CollectionChange CollectionChange { get; set; }
        public K Key { get; set; }
    }

    public sealed class EmployeeDictionary : IDictionary<string, IEmployee>, IObservableMap<string, IEmployee>
    {
        private Dictionary<string, IEmployee> data = new Dictionary<string, IEmployee>();
        #region IObservable MAP
        public event MapChangedEventHandler<string, IEmployee> MapChanged;

        private void FireMapChanged(CollectionChange change, string key)
        {
            if (MapChanged != null)
            {
                MapChanged(this, new MapChangedEventArgs<string>() { CollectionChange = change, Key = key });
            }
        }

        #endregion

        #region Generic Dictionary
        public IEmployee this[string key]
        {
            get
            {
                return data[key];
            }
            set
            {
                data[key] = value;
                FireMapChanged(CollectionChange.ItemChanged, key);
            }
        }

        public int Count { get { return data.Count; } }

        public bool IsReadOnly { get { return false; } }

        public ICollection<string> Keys { get { return data.Keys; } }

        public ICollection<IEmployee> Values { get { return data.Values; } }

        public void Add(KeyValuePair<string, IEmployee> item)
        {
            data.Add(item.Key, item.Value);
            FireMapChanged(CollectionChange.ItemInserted, item.Key);
        }

        public void Add(string key, IEmployee value)
        {
            data.Add(key, value);
            FireMapChanged(CollectionChange.ItemInserted, key);
        }

        public void Clear()
        {
            data.Clear();
            FireMapChanged(CollectionChange.Reset, default(string));
        }

        public bool Contains(KeyValuePair<string, IEmployee> item) { return data.Contains(item); }

        public bool ContainsKey(string key) { return data.ContainsKey(key); }

        public void CopyTo(KeyValuePair<string, IEmployee>[] array, int arrayIndex)
        {
            var keys = data.Keys.GetEnumerator();
            var values = data.Values.GetEnumerator();
            for (int i = 0; i < data.Count; i++)
            {
                KeyValuePair<string, IEmployee> pair = new KeyValuePair<string, IEmployee>(keys.Current, values.Current);
                array[arrayIndex + i] = pair;
                keys.MoveNext();
                values.MoveNext();
            }
        }

        public IEnumerator<KeyValuePair<string, IEmployee>> GetEnumerator() { return data.GetEnumerator(); }

        public bool Remove(KeyValuePair<string, IEmployee> item) { return Remove(item.Key); }

        public bool Remove(string key)
        {
            bool removed = data.Remove(key);
            if (removed) { FireMapChanged(CollectionChange.ItemRemoved, key); }
            return removed;
        }

        public bool TryGetValue(string key, out IEmployee value)
        {
            return data.TryGetValue(key, out value);
        }

        IEnumerator IEnumerable.GetEnumerator() { return ((IEnumerable)data).GetEnumerator(); }
        #endregion

    }

}
