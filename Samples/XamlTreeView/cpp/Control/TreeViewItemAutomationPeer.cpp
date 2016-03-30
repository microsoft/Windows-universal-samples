#pragma once
#include "pch.h"
#include "TreeViewItemAutomationPeer.h"
#include "TreeNode.h"

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;


namespace TreeViewControl {
    //IExpandCollapseProvider
    Windows::UI::Xaml::Automation::ExpandCollapseState TreeViewItemAutomationPeer::ExpandCollapseState::get()
    {
        Windows::UI::Xaml::Automation::ExpandCollapseState currentState = Windows::UI::Xaml::Automation::ExpandCollapseState::Collapsed;
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetParentListView((DependencyObject^)Owner);

        TreeNode^ targetNode;
        TreeNode^ targetParentNode;

        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            targetNode = (TreeNode^)ancestorTreeView->ItemFromContainer((TreeViewItem^)Owner);

            if (Owner->AllowDrop)
            {
                if (targetNode->IsExpanded)
                {
                    currentState = Windows::UI::Xaml::Automation::ExpandCollapseState::Expanded;
                }
                else
                {
                    currentState = Windows::UI::Xaml::Automation::ExpandCollapseState::Collapsed;
                }
            }
            else
            {
                currentState = Windows::UI::Xaml::Automation::ExpandCollapseState::LeafNode;
            }
        }

        return currentState;
    }

    void TreeViewItemAutomationPeer::Collapse()
    {
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetParentListView((DependencyObject^)Owner);

        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            TreeNode^ targetNode = (TreeNode^)ancestorTreeView->ItemFromContainer((TreeViewItem^)Owner);
            ancestorTreeView->CollapseNode(targetNode);
            RaiseExpandCollapseAutomationEvent(Windows::UI::Xaml::Automation::ExpandCollapseState::Collapsed);
        }
    }

    void TreeViewItemAutomationPeer::Expand()
    {
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetParentListView((DependencyObject^)Owner);

        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            TreeNode^ targetNode = (TreeNode^)ancestorTreeView->ItemFromContainer((TreeViewItem^)Owner);
            ancestorTreeView->ExpandNode(targetNode);
            RaiseExpandCollapseAutomationEvent(Windows::UI::Xaml::Automation::ExpandCollapseState::Expanded);
        }
    }

    void TreeViewItemAutomationPeer::RaiseExpandCollapseAutomationEvent(Windows::UI::Xaml::Automation::ExpandCollapseState newState)
    {
        Windows::UI::Xaml::Automation::ExpandCollapseState oldState;

        if (newState == Windows::UI::Xaml::Automation::ExpandCollapseState::Expanded)
        {
            oldState = Windows::UI::Xaml::Automation::ExpandCollapseState::Collapsed;
        }
        else
        {
            oldState = Windows::UI::Xaml::Automation::ExpandCollapseState::Expanded;
        }

        RaisePropertyChangedEvent(ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty, oldState, newState);
    }

    //Position override

    //These methods are being overridden so that the TreeView under narrator reads out
    //the position of an item as compared to it's children, not it's overall position
    //in the listview. I've included an override for level as well, to give context on
    //how deep in the tree an item is.
    int TreeViewItemAutomationPeer::GetSizeOfSetCore()
    {
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetParentListView((DependencyObject^)Owner);

        TreeNode^ targetNode;
        TreeNode^ targetParentNode;

        int setSize = 0;

        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            targetNode = (TreeNode^)ancestorTreeView->ItemFromContainer((TreeViewItem^)Owner);
            targetParentNode = targetNode->ParentNode;
            setSize = targetParentNode->Size;
        }

        return setSize;
    }

    int TreeViewItemAutomationPeer::GetPositionInSetCore()
    {
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetParentListView((DependencyObject^)Owner);

        TreeNode^ targetNode;
        TreeNode^ targetParentNode;

        int positionInSet = 0;

        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            targetNode = (TreeNode^)ancestorTreeView->ItemFromContainer((TreeViewItem^)Owner);
            unsigned int positionInt;
            targetParentNode = targetNode->ParentNode;
            targetParentNode->IndexOf(targetNode, &positionInt);
            positionInSet = (int)positionInt + 1;
        }

        return positionInSet;
    }

    int TreeViewItemAutomationPeer::GetLevelCore()
    {
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = GetParentListView((DependencyObject^)Owner);

        TreeNode^ targetNode;
        TreeNode^ targetParentNode;

        int levelValue = 0;
        
        if (ancestorListView)
        {
            TreeView^ ancestorTreeView = (TreeView^)ancestorListView;
            targetNode = (TreeNode^)ancestorTreeView->ItemFromContainer((TreeViewItem^)Owner);
            levelValue = targetNode->Depth + 1;
        }

        return levelValue;
    }

    Platform::Object^ TreeViewItemAutomationPeer::GetPatternCore(Windows::UI::Xaml::Automation::Peers::PatternInterface patternInterface)
    {
        if (patternInterface == Windows::UI::Xaml::Automation::Peers::PatternInterface::ExpandCollapse)
        {
            return this;
        }

        return ListViewItemAutomationPeer::GetPatternCore(patternInterface);
    }

    Windows::UI::Xaml::Controls::ListView^ TreeViewItemAutomationPeer::GetParentListView(DependencyObject^ Owner)
    {
        DependencyObject^ treeViewItemAncestor = Owner;
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = nullptr;
        while (treeViewItemAncestor != nullptr && ancestorListView == nullptr)
        {
            treeViewItemAncestor = Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(treeViewItemAncestor);
            ancestorListView = dynamic_cast<Windows::UI::Xaml::Controls::ListView^>(treeViewItemAncestor);
        }

        return ancestorListView;
    }
}