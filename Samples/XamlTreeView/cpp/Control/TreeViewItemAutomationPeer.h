#pragma once
#include "TreeViewItem.h"
#include "TreeView.h"

namespace TreeViewControl {
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class TreeViewItemAutomationPeer : Windows::UI::Xaml::Automation::Peers::ListViewItemAutomationPeer, Windows::UI::Xaml::Automation::Provider::IExpandCollapseProvider
    {
    internal:
        TreeViewItemAutomationPeer(TreeViewItem^ owner) :Windows::UI::Xaml::Automation::Peers::ListViewItemAutomationPeer(owner) {};

    public:
        //IExpandCollapseProvider
        virtual void Collapse();

        virtual void Expand();

        property Windows::UI::Xaml::Automation::ExpandCollapseState ExpandCollapseState
        {
            virtual Windows::UI::Xaml::Automation::ExpandCollapseState get();
        }

        void RaiseExpandCollapseAutomationEvent(Windows::UI::Xaml::Automation::ExpandCollapseState newState);

        //Position override
        int GetSizeOfSetCore() override;

        int GetPositionInSetCore() override;

        int GetLevelCore() override;

    protected:
        Platform::Object^ GetPatternCore(Windows::UI::Xaml::Automation::Peers::PatternInterface patternInterface) override;

    private:
        Windows::UI::Xaml::Controls::ListView^ GetParentListView(DependencyObject^ Owner);
    };
}

