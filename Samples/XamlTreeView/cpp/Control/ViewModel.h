#pragma once
#include "TreeNode.h"

namespace TreeViewControl {
    /// <summary>
    /// The ViewModel is responsible for flattening the heirarchical data into a flat list.
    /// It also tracks changes to the underlying data and updates the flat list accordingly.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    ref class ViewModel sealed : Windows::UI::Xaml::Interop::IBindableObservableVector
    {
    internal:
        ViewModel();

    public:
        /// <summary>
        /// Add root to the view model. In other cases the app should not 
        /// use this api.
        /// </summary>
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

        virtual property unsigned int Size
        {
            unsigned int get() { return flatVectorRealizedItems->Size; };
        };

        virtual event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ VectorChanged
        {
            virtual Windows::Foundation::EventRegistrationToken add(Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ args)
            {
                return ViewModelChanged += args;
            }

            virtual void remove(Windows::Foundation::EventRegistrationToken token)
            {
                return ViewModelChanged -= token;
            }

            virtual void raise(Windows::UI::Xaml::Interop::IBindableObservableVector^ vector, Platform::Object^ e)
            {
                ViewModelChanged(vector, e);
            }
        }

        /// <summary>
        /// ExpandNode adds the children and all open descendants of the targetNode 
        /// to the ViewModel in their correct flattened index.
        /// </summary>
        void ExpandNode(TreeNode^ targetNode);

        /// <summary>
        /// Collapse node removes all the descendants of the targetNode from the ViewModel.
        /// </summary>
        void CollapseNode(TreeNode^ targetNode);

        /// <summary>
        /// This is the collection changed handler for individual TreeNodes. The collection changes
        /// from the hierarchical TreeNodes need to be flattened so that we can keep our current 
        /// flat list in sync.
        /// </summary>
        /// <param name="sender">TreeNode that has already been changed</param>
        void TreeNodeVectorChanged(Windows::UI::Xaml::Interop::IBindableObservableVector^ sender, Platform::Object^ e);

        void TreeNodePropertyChanged(Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);

    private:
        event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ ViewModelChanged;

        Platform::Collections::Vector<TreeNode^>^ flatVectorRealizedItems = ref new Platform::Collections::Vector<TreeNode^>();

        std::vector<Windows::Foundation::EventRegistrationToken> collectionChangedEventTokenVector;

        std::vector<Windows::Foundation::EventRegistrationToken> propertyChangedEventTokenVector;

        void AddNodeToView(TreeNode^ targetNode, int index);

        int AddNodeDescendantsToView(TreeNode^ targetNode, int start, int offset);

        void RemoveNodeAndDescendantsFromView(TreeNode^ targetNode);

        int CountDescendants(TreeNode^ targetNode);

        int IndexOf(TreeNode^ targetNode);

        void UpdateTreeView(Windows::Foundation::Collections::IObservableVector<TreeNode^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ e);
    };
}