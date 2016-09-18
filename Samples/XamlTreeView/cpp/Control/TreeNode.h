#pragma once

namespace TreeViewControl {
    /// <summary>
    /// The TreeNode class implements the hierarchical layout for the TreeView.
    /// It also holds the data that will be bound to in the item template.
    /// </summary>
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class TreeNode sealed : Windows::UI::Xaml::Interop::IBindableObservableVector, Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        TreeNode();

        virtual void Append(Object^ value);

        virtual void Clear();

        virtual Windows::UI::Xaml::Interop::IBindableIterator^ First();

        virtual Object^ GetAt(unsigned int index);

        virtual Windows::UI::Xaml::Interop::IBindableVectorView^ GetView();

        virtual bool IndexOf(Object^ value, unsigned int* index);

        virtual void InsertAt(unsigned int index, Object^ value);

        virtual void RemoveAt(unsigned int index);

        virtual void RemoveAtEnd();

        virtual void SetAt(unsigned int index, Object^ value);

        virtual event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ VectorChanged
        {
            virtual Windows::Foundation::EventRegistrationToken add(Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ e)
            {
                return TreeNodeChanged += e;
            }

            virtual void remove(Windows::Foundation::EventRegistrationToken t)
            {
                TreeNodeChanged -= t;
            }

            internal: virtual void raise(Windows::UI::Xaml::Interop::IBindableObservableVector^ vector, Platform::Object^ e)
            {
                TreeNodeChanged(vector, e);
            }
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        virtual property unsigned int Size
        {
            unsigned int get();
        }

        property Object^ Data
        {
            Object^ get();
            void set(Object^ value);
        }

        property TreeNode^ ParentNode
        {
            TreeNode^ get();
            void set(TreeNode^ value);
        }

        property bool IsExpanded
        {
            bool get();
            void set(bool value);
        }

        property bool HasItems
        {
            bool get();
        }

        //A lone TreeNode will have a depth of -1, this is to show that it is not appended
        //under the TreeView's invisible root node. Once added into the TreeView via
        //that method, the depth of the node will be calculated appropriately.
        property int Depth
        {
            int get();
        }

        event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ TreeNodeChanged;

    private:
        TreeNode^ parentNode = nullptr;
        Object^ data = nullptr;
        bool isExpanded = false;
        Platform::Collections::Vector<TreeNode^>^ childrenVector = ref new Platform::Collections::Vector<TreeNode^>();
        void ChildrenVectorChanged(Windows::Foundation::Collections::IObservableVector<TreeNode^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ e);
    };
}
