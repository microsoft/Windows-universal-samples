#pragma once
#include "pch.h"
#include "TreeNode.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace TreeViewControl {

    TreeNode::TreeNode()
    {
        childrenVector->VectorChanged += ref new VectorChangedEventHandler<TreeNode ^>(this, &TreeNode::ChildrenVectorChanged);
    }

    void TreeNode::Append(Object^ value)
    {
        int count = childrenVector->Size;
        TreeNode^ targetNode = (TreeNode^)value;
        targetNode->ParentNode = this;
        childrenVector->Append(targetNode);

        //If the count was 0 before we appended, then the HasItems property needs to change.
        if (count == 0)
        {
            this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("HasItems"));
        }

        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Size"));
    }

    void TreeNode::Clear()
    {
        int count = childrenVector->Size;
        TreeNode^ childNode;
        for (int i = 0; i < (int)Size; i++)
        {
            childNode = (TreeNode^)GetAt(i);
            childNode->ParentNode = nullptr;
        }

        childrenVector->Clear();

        //If the count was not 0 before we cleared, then the HasItems property needs to change.
        if (count != 0)
        {
            this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("HasItems"));
        }

        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Size"));
    }

    IBindableIterator^ TreeNode::First()
    {
        return dynamic_cast<IBindableIterator^>(childrenVector->First());
    }

    Object^ TreeNode::GetAt(unsigned int index)
    {
        if ((int)index > -1 && index < childrenVector->Size)
        {
            return childrenVector->GetAt(index);
        }

        return nullptr;
    }

    IBindableVectorView^ TreeNode::GetView()
    {
        return safe_cast<IBindableVectorView^>(childrenVector->GetView());
    }

    bool TreeNode::IndexOf(Object^ value, unsigned int* index)
    {
        return childrenVector->IndexOf((TreeNode^)value, index);
    }

    void TreeNode::InsertAt(unsigned int index, Object^ value)
    {
        if ((int)index > -1 && index <= childrenVector->Size)
        {
            int count = childrenVector->Size;
            TreeNode^ targetNode = (TreeNode^)value;
            targetNode->ParentNode = this;
            return childrenVector->InsertAt(index, (TreeNode^)value);

            //If the count was 0 before we insert, then the HasItems property needs to change.
            if (count == 0)
            {
                this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("HasItems"));
            }

            this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Size"));
        }
    }

    void TreeNode::RemoveAt(unsigned int index)
    {
        if ((int)index > -1 && index < childrenVector->Size)
        {
            int count = childrenVector->Size;
            TreeNode^ targetNode = childrenVector->GetAt(index);
            targetNode->ParentNode = nullptr;
            childrenVector->RemoveAt(index);

            //If the count was 1 before we remove, then the HasItems property needs to change.
            if (count == 1)
            {
                this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("HasItems"));
            }

            this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Size"));
        }
    }

    void TreeNode::RemoveAtEnd()
    {
        int count = childrenVector->Size;
        TreeNode^ targetNode = childrenVector->GetAt(childrenVector->Size - 1);
        targetNode->ParentNode = nullptr;
        childrenVector->RemoveAtEnd();

        //If the count was 1 before we remove, then the HasItems property needs to change.
        if (count == 1)
        {
            this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("HasItems"));
        }

        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Size"));
    }

    void TreeNode::SetAt(unsigned int index, Object^ value)
    {
        if ((int)index > -1 && index <= childrenVector->Size)
        {
            childrenVector->GetAt(index)->ParentNode = nullptr;
            TreeNode^ targetNode = (TreeNode^)value;
            targetNode->ParentNode = this;
            return childrenVector->SetAt(index, targetNode);
        }
    }

    void TreeNode::ChildrenVectorChanged(IObservableVector<TreeNode^>^ sender, IVectorChangedEventArgs^ e)
    {
        VectorChanged(this, e);
    }

    unsigned int TreeNode::Size::get()
    {
        return childrenVector->Size;
    }

    Object^ TreeNode::Data::get()
    {
        return data;
    }

    void TreeNode::Data::set(Object^ value)
    {
        data = value;
        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Data"));
    }

    TreeNode^ TreeNode::ParentNode::get()
    {
        return parentNode;
    }

    void TreeNode::ParentNode::set(TreeNode^ value)
    {
        parentNode = value;
        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("ParentNode"));
        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Depth"));
    }

    bool TreeNode::IsExpanded::get()
    {
        return isExpanded;
    }

    void TreeNode::IsExpanded::set(bool value)
    {
        isExpanded = value;
        this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("IsExpanded"));
    }

    bool TreeNode::HasItems::get()
    {
        return (Size != 0);
    }

    int TreeNode::Depth::get()
    {
        TreeNode^ ancestorNode = this;
        int depth = -1;
        while ((ancestorNode->ParentNode) != nullptr)
        {
            depth++;
            ancestorNode = ancestorNode->ParentNode;
        }

        return depth;
    }

}