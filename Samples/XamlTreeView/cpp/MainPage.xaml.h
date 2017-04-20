//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "BooleanToVisibilityConverter.h"
#include "GlyphConverter.h"
#include "FileSystemData.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();

    private:
        void SampleTreeView_ContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs^ args);
        TreeViewControl::TreeNode^ CreateFileNode(Platform::String^ name);
        TreeViewControl::TreeNode^ CreateFolderNode(Platform::String^ name);
    };
}
