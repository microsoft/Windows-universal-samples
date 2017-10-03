//
// VideoGallery.xaml.h
// Declaration of the VideoGallery class
//

#pragma once

#include "VideoGallery.g.h"
#include "VideoItem.h"

namespace _360VideoPlayback
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class VideoGallery sealed
    {
    public:
        VideoGallery();
        static int GetMainViewId() { return mainviewId; }

    private:
        void GalleryGridView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
        void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        static int mainviewId;
    };
}
