//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace TreeViewControl;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    InitializeComponent();

    TreeNode^ workFolder = CreateFolderNode("Work Documents");
    workFolder->Append(CreateFileNode("Feature Functional Spec"));
    workFolder->Append(CreateFileNode("Feature Schedule"));
    workFolder->Append(CreateFileNode("Overall Project Plan"));
    workFolder->Append(CreateFileNode("Feature Resource allocation"));
    sampleTreeView->RootNode->Append(workFolder);

    auto remodelFolder = CreateFolderNode("Home Remodel");
    remodelFolder->IsExpanded = true;
    remodelFolder->Append(CreateFileNode("Contactor Contact Information"));
    remodelFolder->Append(CreateFileNode("Paint Color Scheme"));
    remodelFolder->Append(CreateFileNode("Flooring woodgrain types"));
    remodelFolder->Append(CreateFileNode("Kitchen cabinet styles"));

    auto personalFolder = CreateFolderNode("Personal Documents");
    personalFolder->IsExpanded = true;
    personalFolder->Append(remodelFolder);
    sampleTreeView->RootNode->Append(personalFolder);

    sampleTreeView->ContainerContentChanging += ref new TypedEventHandler<ListViewBase^, ContainerContentChangingEventArgs^>(this, &MainPage::SampleTreeView_ContainerContentChanging);
}

void MainPage::SampleTreeView_ContainerContentChanging(ListViewBase^ sender, ContainerContentChangingEventArgs^ args)
{
    auto node = dynamic_cast<TreeNode^>(args->Item);
    if (node != nullptr)
    {
        auto data = dynamic_cast<FileSystemData^>(node->Data);
        if (data != nullptr)
        {
            args->ItemContainer->AllowDrop = data->IsFolder;
        }
    }
}

TreeNode^ MainPage::CreateFileNode(String^ name)
{
    auto data = ref new FileSystemData(name);
    auto treeNode = ref new TreeNode();
    treeNode->Data = data;
    return treeNode;
}

TreeNode^ MainPage::CreateFolderNode(String^ name)
{
    auto data = ref new FileSystemData(name);
    data->IsFolder = true;
    auto treeNode = ref new TreeNode();
    treeNode->Data = data;
    return treeNode;
}