#pragma once
#include "pch.h"
#include "TreeViewItem.h"
#include "TreeViewItemAutomationPeer.h"

using namespace Windows::UI::Xaml;
using namespace Platform;

namespace TreeViewControl {
    TreeViewItem::TreeViewItem()
    {
    }

    TreeViewItem::~TreeViewItem()
    {
    }

    Windows::UI::Xaml::Controls::ListView^ TreeViewItem::GetAncestorListView(TreeViewItem^ targetItem)
    {
        DependencyObject^ TreeViewItemAncestor = (DependencyObject^)this;
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = nullptr;
        while (TreeViewItemAncestor != nullptr && ancestorListView == nullptr)
        {
            TreeViewItemAncestor = Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(TreeViewItemAncestor);
            ancestorListView = dynamic_cast<Windows::UI::Xaml::Controls::ListView^>(TreeViewItemAncestor);
        }
        return ancestorListView;
    }

    void TreeViewItem::OnDrop(Windows::UI::Xaml::DragEventArgs^ e)
    {
        if (e->AcceptedOperation == Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move)
        {
            TreeViewItem^ droppedOnItem = (TreeViewItem^)this;

            Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetAncestorListView(droppedOnItem);

            if (ancestorListView)
            {
                TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
                TreeViewItem^ droppedItem = ancestorTreeView->draggedTreeViewItem;
                TreeNode^ droppedNode = (TreeNode^)ancestorTreeView->ItemFromContainer(droppedItem);
                TreeNode^ droppedOnNode = (TreeNode^)ancestorTreeView->ItemFromContainer(droppedOnItem);

                //Remove the item that was dragged
                unsigned int removeIndex;
                droppedNode->ParentNode->IndexOf(droppedNode, &removeIndex);

                if (droppedNode != droppedOnNode)
                {
                    droppedNode->ParentNode->RemoveAt(removeIndex);

                    //Append the dragged dropped item as a child of the node it was dropped onto
                    droppedOnNode->Append(droppedNode);

                    //If not set to true then the Reorder code of listview wil override what is being done here.
                    e->Handled = true;
                }
                else
                {
                    e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::None;
                }
            }
        }
    }

    void TreeViewItem::OnDragEnter(Windows::UI::Xaml::DragEventArgs^ e)
    {
        TreeViewItem^ draggedOverItem = (TreeViewItem^)this;

        e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::None;

        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetAncestorListView(draggedOverItem);

        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            TreeViewItem^ draggedTreeViewItem = ancestorTreeView->draggedTreeViewItem;
            TreeNode^ draggedNode = (TreeNode^)ancestorTreeView->ItemFromContainer(draggedTreeViewItem);
            TreeNode^ draggedOverNode = (TreeNode^)ancestorTreeView->ItemFromContainer(draggedOverItem);
            TreeNode^ walkNode = draggedOverNode->ParentNode;

            while (walkNode != nullptr && walkNode != draggedNode)
            {
                walkNode = walkNode->ParentNode;
            }

            if (walkNode != draggedNode && draggedNode != draggedOverNode)
            {
                e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move;
            }
        }
    }

    void TreeViewItem::OnDragOver(Windows::UI::Xaml::DragEventArgs^ e)
    {
        e->DragUIOverride->IsGlyphVisible = true;
        e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::None;

        TreeViewItem^ draggedOverItem = (TreeViewItem^)this;

        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetAncestorListView(draggedOverItem);
        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            TreeViewItem^ draggedTreeViewItem = ancestorTreeView->draggedTreeViewItem;
            TreeNode^ draggedNode = (TreeNode^)ancestorTreeView->ItemFromContainer(draggedTreeViewItem);
            TreeNode^ draggedOverNode = (TreeNode^)ancestorTreeView->ItemFromContainer(draggedOverItem);
            TreeNode^ walkNode = draggedOverNode->ParentNode;

            while (walkNode != nullptr && walkNode != draggedNode)
            {
                walkNode = walkNode->ParentNode;
            }

            if (walkNode != draggedNode && draggedNode != draggedOverNode)
            {
                e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move;
            }
        }
    }

    Windows::UI::Xaml::Automation::Peers::AutomationPeer^ TreeViewItem::OnCreateAutomationPeer()
    {
        return ref new TreeViewItemAutomationPeer(this);
    }
}