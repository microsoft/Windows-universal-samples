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
}
