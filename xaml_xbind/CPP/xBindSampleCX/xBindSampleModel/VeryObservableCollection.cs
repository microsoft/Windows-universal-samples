using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;

namespace BindTestbedModel
{
    internal class VeryObservableCollection<T> : INotifyCollectionChanged, IList<T>, Windows.Foundation.Collections.IObservableVector<T>, IList
    {
        private ObservableCollection<T> data;

        public VeryObservableCollection()
        {
            data = new ObservableCollection<T>();
            data.CollectionChanged += Data_CollectionChanged;
        }

        #region NotifyCollectionChanged

        public event VectorChangedEventHandler<T> VectorChanged;

        public event NotifyCollectionChangedEventHandler CollectionChanged;

        private void Data_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (VectorChanged != null)
            {
                VectorChangedEventAgrs args = new VectorChangedEventAgrs();
                args.CollectionChange = CollectionChange.Reset;
                VectorChanged(this, args);
            }

            if (CollectionChanged != null)
            {
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                CollectionChanged(this, e);
            }
        }

        #endregion
        #region IList<T>

        public T this[int index]
        {
            get { return data[index]; }
            set { data[index] = value; }
        }

        public int Count { get { return data.Count; } }

        public bool IsReadOnly
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public void Add(T item)
        {
            data.Add(item);
        }

        public void Clear()
        {
            data.Clear();
        }

        public bool Contains(T item)
        {
            return data.Contains(item);
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            data.CopyTo(array, arrayIndex);
        }

        public IEnumerator<T> GetEnumerator()
        {
            return data.GetEnumerator();
        }

        public int IndexOf(T item)
        {
            return data.IndexOf(item);
        }

        public void Insert(int index, T item)
        {
            data.Insert(index, item);
        }

        public bool Remove(T item)
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
        #region IList

        bool IList.IsFixedSize { get { return false; } }

        bool IList.IsReadOnly { get { return false; } }

        int ICollection.Count { get { return data.Count; } }
        bool ICollection.IsSynchronized { get { return false; } }

        object ICollection.SyncRoot { get { throw new NotImplementedException(); } }

        object IList.this[int index]
        {
            get { return data[index]; }
            set { data[index] = (T)value; }
        }
        int IList.Add(object item)
        {
            data.Add((T)item);
            return data.Count;
        }

        bool IList.Contains(object item) { return data.Contains((T)item); }

        int IList.IndexOf(object item) { return data.IndexOf((T)item); }


        void IList.Insert(int index, object item)
        {
            data.Insert(index, (T)item);
        }

        void IList.Remove(object item)
        {
            data.Remove((T)item);
        }
        void IList.Clear()
        {
            data.Clear();
        }

        void IList.RemoveAt(int index)
        {
            data.RemoveAt(index);
        }

        void ICollection.CopyTo(Array dest, int index)
        {
            data.CopyTo((T[])dest, index);
        }
        #endregion
           }
}

