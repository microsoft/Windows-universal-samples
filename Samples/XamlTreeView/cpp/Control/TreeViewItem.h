#pragma once

namespace TreeViewControl {
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class TreeViewItem sealed : Windows::UI::Xaml::Controls::ListViewItem
    {
    public:
        TreeViewItem();

        virtual ~TreeViewItem();

    private:
        Windows::UI::Xaml::Controls::ListView^ GetAncestorListView(TreeViewItem^ targetItem);

    protected:
        void OnDrop(Windows::UI::Xaml::DragEventArgs^ e) override;
        void OnDragEnter(Windows::UI::Xaml::DragEventArgs^ e) override;
        void OnDragOver(Windows::UI::Xaml::DragEventArgs^ e) override;
        Windows::UI::Xaml::Automation::Peers::AutomationPeer^ OnCreateAutomationPeer() override;
    };
}