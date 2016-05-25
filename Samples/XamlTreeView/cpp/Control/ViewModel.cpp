#pragma once
#include "pch.h"
#include "ViewModel.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Data;

namespace TreeViewControl {
    ViewModel::ViewModel()
    {
        flatVectorRealizedItems->VectorChanged += ref new VectorChangedEventHandler<TreeNode ^>(this, &ViewModel::UpdateTreeView);
    }

    void ViewModel::Append(Object^ value)
    {
        TreeNode^ targetNode = (TreeNode^)value;
        flatVectorRealizedItems->Append(targetNode);

        collectionChangedEventTokenVector.push_back(targetNode->VectorChanged += ref new BindableVectorChangedEventHandler(this, &ViewModel::TreeNodeVectorChanged));
        propertyChangedEventTokenVector.push_back(targetNode->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &ViewModel::TreeNodePropertyChanged));
    }

    void ViewModel::Clear()
    {

        while (flatVectorRealizedItems->Size != 0)
        {
            RemoveAtEnd();
        }
    }

    IBindableIterator^ ViewModel::First()
    {
        return dynamic_cast<IBindableIterator^>(flatVectorRealizedItems->First());
    }

    Object^ ViewModel::GetAt(unsigned int index)
    {
        if ((int)index > -1 && (int)index < flatVectorRealizedItems->Size)
        {
            return flatVectorRealizedItems->GetAt(index);
        }

        return nullptr;
    }

    IBindableVectorView^ ViewModel::GetView()
    {
        return safe_cast<IBindableVectorView^>(flatVectorRealizedItems->GetView());
    }

    bool ViewModel::IndexOf(Object^ value, unsigned int* index)
    {
        return flatVectorRealizedItems->IndexOf((TreeNode^)value, index);
    }

    void ViewModel::InsertAt(unsigned int index, Object^ value)
    {
        if ((int)index > -1 && (int)index <= flatVectorRealizedItems->Size)
        {
            TreeNode^ targetNode = (TreeNode^)value;
            flatVectorRealizedItems->InsertAt(index, targetNode);

            auto eventIndex = collectionChangedEventTokenVector.begin() + index;
            collectionChangedEventTokenVector.insert(eventIndex, targetNode->VectorChanged += ref new BindableVectorChangedEventHandler(this, &ViewModel::TreeNodeVectorChanged));

            eventIndex = propertyChangedEventTokenVector.begin() + index;
            propertyChangedEventTokenVector.insert(eventIndex,targetNode->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &ViewModel::TreeNodePropertyChanged));
        }
    }

    void ViewModel::RemoveAt(unsigned int index)
    {
        if ((int)index > -1 && (int)index < flatVectorRealizedItems->Size)
        {
            TreeNode^ targetNode = flatVectorRealizedItems->GetAt(index);
            flatVectorRealizedItems->RemoveAt(index);

            auto eventIndex = collectionChangedEventTokenVector.begin() + index;
            targetNode->VectorChanged -= collectionChangedEventTokenVector[index];
            collectionChangedEventTokenVector.erase(eventIndex);

            eventIndex = propertyChangedEventTokenVector.begin() + index;
            targetNode->PropertyChanged -= propertyChangedEventTokenVector[index];
            propertyChangedEventTokenVector.erase(eventIndex);
        }
    }

    void ViewModel::RemoveAtEnd()
    {
        int index = flatVectorRealizedItems->Size - 1;
        if (index >= 0)
        {
            TreeNode^ targetNode = flatVectorRealizedItems->GetAt(index);
            flatVectorRealizedItems->RemoveAt(index);

            auto eventIndex = collectionChangedEventTokenVector.begin() + index;
            targetNode->VectorChanged -= collectionChangedEventTokenVector[index];
            collectionChangedEventTokenVector.erase(eventIndex);

            eventIndex = propertyChangedEventTokenVector.begin() + index;
            targetNode->PropertyChanged -= propertyChangedEventTokenVector[index];
            propertyChangedEventTokenVector.erase(eventIndex);
        }
    }

    void ViewModel::SetAt(unsigned int index, Object^ value)
    {
        if ((int)index > -1 && (int)index < flatVectorRealizedItems->Size)
        {
            TreeNode^ targetNode = (TreeNode^)value;
            TreeNode^ removeNode = flatVectorRealizedItems->GetAt(index);
            flatVectorRealizedItems->SetAt(index, targetNode);

            auto eventIndex = collectionChangedEventTokenVector.begin() + index;
            removeNode->VectorChanged -= collectionChangedEventTokenVector[index];
            collectionChangedEventTokenVector.erase(eventIndex);
            collectionChangedEventTokenVector.insert(eventIndex, targetNode->VectorChanged += ref new BindableVectorChangedEventHandler(this, &ViewModel::TreeNodeVectorChanged));

            eventIndex = propertyChangedEventTokenVector.begin() + index;
            targetNode->PropertyChanged -= propertyChangedEventTokenVector[index];
            propertyChangedEventTokenVector.erase(eventIndex);
            propertyChangedEventTokenVector.insert(eventIndex, targetNode->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &ViewModel::TreeNodePropertyChanged));
        }
    }

    void ViewModel::ExpandNode(TreeNode^ targetNode)
    {
        if (!targetNode->IsExpanded)
        {
            targetNode->IsExpanded = true;
        }
    }

    void ViewModel::CollapseNode(TreeNode^ targetNode)
    {
        if (targetNode->IsExpanded)
        {
            targetNode->IsExpanded = false;
        }
    }

    void ViewModel::AddNodeToView(TreeNode^ targetNode, int index)
    {
        InsertAt(index, targetNode);
    }

    int ViewModel::AddNodeDescendantsToView(TreeNode^ targetNode, int index, int offset)
    {
        if (targetNode->IsExpanded)
        {
            TreeNode^ childNode;
            for (int i = 0; i < (int)targetNode->Size; i++)
            {
                childNode = (TreeNode^)targetNode->GetAt(i);
                offset++;
                AddNodeToView(childNode, index + offset);
                offset = AddNodeDescendantsToView(childNode, index, offset);
            }

            return offset;
        }

        return offset;
    }

    void ViewModel::RemoveNodeAndDescendantsFromView(TreeNode^ targetNode)
    {
        if (targetNode->IsExpanded)
        {
            TreeNode^ childNode;
            for (int i = 0; i < (int)targetNode->Size; i++)
            {
                childNode = (TreeNode^)targetNode->GetAt(i);
                RemoveNodeAndDescendantsFromView(childNode);
            }
        }

        int index = IndexOf(targetNode);
        RemoveAt(index);
    }

    int ViewModel::CountDescendants(TreeNode^ targetNode)
    {
        int descendantCount = 0;
        TreeNode^ childNode;
        for (int i = 0; i < (int)targetNode->Size; i++)
        {
            childNode = (TreeNode^)targetNode->GetAt(i);
            descendantCount++;
            if (childNode->IsExpanded)
            {
                descendantCount = descendantCount + CountDescendants(childNode);
            }
        }

        return descendantCount;
    }

    int ViewModel::IndexOf(TreeNode^ targetNode)
    {
        unsigned int index;
        bool isIndexed = IndexOf(targetNode, &index);
        if (isIndexed)
        {
            return (int)index;
        }
        else
        {
            return -1;
        }
    }

    void ViewModel::UpdateTreeView(IObservableVector<TreeNode^>^ sender, IVectorChangedEventArgs^ e)
    {
        VectorChanged(this, e);
    }

    void ViewModel::TreeNodeVectorChanged(IBindableObservableVector^ sender, Platform::Object^ e)
    {
        CollectionChange CC = ((IVectorChangedEventArgs^)e)->CollectionChange;
        switch (CC)
        {
            //Reset case, commonly seen when a TreeNode is cleared.
            //removes all nodes that need removing then 
            //toggles a collapse / expand to ensure order.
        case (CollectionChange::Reset) :
        {
            TreeNode^ resetNode = (TreeNode^)sender;
            int resetIndex = IndexOf(resetNode);
            if (resetIndex != Size - 1 && resetNode->IsExpanded)
            {
                TreeNode^ childNode = resetNode;
                TreeNode^ parentNode = resetNode->ParentNode;
                int stopIndex;
                bool isLastRelativeChild = true;
                while (parentNode != nullptr && isLastRelativeChild)
                {
                    unsigned int relativeIndex;
                    parentNode->IndexOf(childNode, &relativeIndex);
                    if (parentNode->Size - 1 != relativeIndex)
                    {
                        isLastRelativeChild = false;
                    }
                    else
                    {
                        childNode = parentNode;
                        parentNode = parentNode->ParentNode;
                    }
                }

                if (parentNode != nullptr)
                {
                    unsigned int siblingIndex;
                    parentNode->IndexOf(childNode, &siblingIndex);
                    TreeNode^ siblingNode = (TreeNode^)parentNode->GetAt(siblingIndex + 1);
                    stopIndex = IndexOf(siblingNode);
                }
                else
                {
                    stopIndex = Size;
                }

                for (int i = stopIndex - 1; i > resetIndex; i--)
                {
                    if ((flatVectorRealizedItems->GetAt(i))->ParentNode == nullptr)
                    {
                        RemoveNodeAndDescendantsFromView(flatVectorRealizedItems->GetAt(i));
                    }
                }

                if (resetNode->IsExpanded)
                {
                    CollapseNode(resetNode);
                    ExpandNode(resetNode);
                }
            }

            break;
        }

                                       //Inserts the TreeNode into the correct index of the ViewModel
        case (CollectionChange::ItemInserted) :
        {
            //We will find the correct index of insertion by first checking if the
            //node we are inserting into is expanded. If it is we will start walking
            //down the tree and counting the open items. This is to ensure we place
            //the inserted item in the correct index. If along the way we bump into
            //the item being inserted, we insert there then return, because we don't
            //need to do anything further.
            unsigned int index = ((IVectorChangedEventArgs^)e)->Index;
            TreeNode^ targetNode = (TreeNode^)sender->GetAt(index);
            TreeNode^ parentNode = targetNode->ParentNode;
            TreeNode^ childNode;
            int parentIndex = IndexOf(parentNode);
            int allOpenedDescendantsCount = 0;
            if (parentNode->IsExpanded)
            {
                for (int i = 0; i < (int)parentNode->Size; i++)
                {
                    childNode = (TreeNode^)parentNode->GetAt(i);
                    if (childNode == targetNode)
                    {
                        AddNodeToView(targetNode, (parentIndex + i + 1 + allOpenedDescendantsCount));
                        if (targetNode->IsExpanded)
                        {
                            AddNodeDescendantsToView(targetNode, parentIndex + i + 1, allOpenedDescendantsCount);
                        }

                        return;
                    }

                    if (childNode->IsExpanded)
                    {
                        allOpenedDescendantsCount += CountDescendants(childNode);
                    }
                }

                AddNodeToView(targetNode, (parentIndex + parentNode->Size + allOpenedDescendantsCount));
                if (targetNode->IsExpanded)
                {
                    AddNodeDescendantsToView(targetNode, parentIndex + parentNode->Size, allOpenedDescendantsCount);
                }
            }

            break;
        }

                                              //Removes a node from the ViewModel when a TreeNode
                                              //removes a child.
        case (CollectionChange::ItemRemoved) :
        {
            TreeNode^ removeNode = (TreeNode^)sender;
            int removeIndex = IndexOf(removeNode);

            if (removeIndex != Size - 1 && removeNode->IsExpanded)
            {
                TreeNode^ childNode = removeNode;
                TreeNode^ parentNode = removeNode->ParentNode;
                int stopIndex;
                bool isLastRelativeChild = true;
                while (parentNode != nullptr && isLastRelativeChild)
                {
                    unsigned int relativeIndex;
                    parentNode->IndexOf(childNode, &relativeIndex);
                    if (parentNode->Size - 1 != relativeIndex)
                    {
                        isLastRelativeChild = false;
                    }
                    else
                    {
                        childNode = parentNode;
                        parentNode = parentNode->ParentNode;
                    }
                }

                if (parentNode != nullptr)
                {
                    unsigned int siblingIndex;
                    parentNode->IndexOf(childNode, &siblingIndex);
                    TreeNode^ siblingNode = (TreeNode^)parentNode->GetAt(siblingIndex + 1);
                    stopIndex = IndexOf(siblingNode);
                }
                else
                {
                    stopIndex = Size;
                }

                for (int i = stopIndex - 1; i > removeIndex; i--)
                {
                    if ((flatVectorRealizedItems->GetAt(i))->ParentNode == nullptr)
                    {
                        RemoveNodeAndDescendantsFromView(flatVectorRealizedItems->GetAt(i));
                    }
                }
            }

            break;
        }

                                             //Triggered by a replace such as SetAt.
                                             //Updates the TreeNode that changed in the ViewModel.
        case (CollectionChange::ItemChanged) :
        {
            unsigned int index = ((IVectorChangedEventArgs^)e)->Index;
            TreeNode^ targetNode = (TreeNode^)sender->GetAt(index);
            TreeNode^ parentNode = targetNode->ParentNode;
            TreeNode^ childNode;
            int allOpenedDescendantsCount = 0;
            int parentIndex = IndexOf(parentNode);

            for (int i = 0; i < (int)parentNode->Size; i++)
            {
                childNode = (TreeNode^)parentNode->GetAt(i);
                if (childNode->IsExpanded)
                {
                    allOpenedDescendantsCount += CountDescendants(childNode);
                }
            }

            TreeNode^ removeNode = (TreeNode^)GetAt(parentIndex + index + allOpenedDescendantsCount + 1);
            if (removeNode->IsExpanded)
            {
                CollapseNode(removeNode);
            }

            RemoveAt(parentIndex + index + allOpenedDescendantsCount + 1);
            InsertAt(parentIndex + index + allOpenedDescendantsCount + 1, targetNode);

            break;
        }

        }
    }

    void ViewModel::TreeNodePropertyChanged(Object^ sender, PropertyChangedEventArgs^ e)
    {        
        if (e->PropertyName == "IsExpanded")
        {
            TreeNode^ targetNode = (TreeNode^)sender;
            if (targetNode->IsExpanded)
            {
                if (targetNode->Size != 0)
                {
                    int openedDescendantOffset = 0;
                    int index = IndexOf(targetNode);
                    index = index + 1;
                    TreeNode^ childNode;
                    for (int i = 0; i < (int)targetNode->Size; i++)
                    {
                        childNode = (TreeNode^)targetNode->GetAt(i);
                        AddNodeToView(childNode, ((int)index + i + openedDescendantOffset));
                        openedDescendantOffset = AddNodeDescendantsToView(childNode, (index + i), openedDescendantOffset);
                    }
                }
            }
            else
            {
                TreeNode^ childNode;
                for (int i = 0; i < (int)targetNode->Size; i++)
                {
                    childNode = (TreeNode^)targetNode->GetAt(i);
                    RemoveNodeAndDescendantsFromView(childNode);
                }
            }
        }
    }
}