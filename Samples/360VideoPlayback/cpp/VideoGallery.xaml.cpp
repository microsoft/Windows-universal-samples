//
// VideoGallery.xaml.cpp
// Implementation of the VideoGallery class
//

#include "pch.h"
#include "VideoGallery.xaml.h"
#include "PlaybackPage.xaml.h"
#include "AppView.h"

using namespace _360VideoPlayback;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial::Surfaces;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

int _360VideoPlayback::VideoGallery::mainviewId = 0;

VideoGallery::VideoGallery()
{
    InitializeComponent();
    mainviewId = ApplicationView::GetForCurrentView()->Id;
    this->Loaded += ref new RoutedEventHandler(this, &_360VideoPlayback::VideoGallery::OnLoaded);
}


void _360VideoPlayback::VideoGallery::GalleryGridView_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
    if (e->ClickedItem != nullptr)
    {
        VideoItem^ selectedVideoItem = safe_cast<VideoItem^>(e->ClickedItem);

        if ((HolographicSpace::IsSupported && HolographicSpace::IsAvailable)
            || SpatialSurfaceObserver::IsSupported())
        {
            // If VR or Hololens(or emulator) all attached to then it will play in immersive mode.
            // Make sure enable Mixed Reality portal on the PC
            auto appViewSource = ref new AppViewSource(selectedVideoItem->SourceUri);
            auto appView = CoreApplication::CreateNewView(appViewSource);
            appView->Dispatcher->RunAsync(
                CoreDispatcherPriority::Normal,
                ref new DispatchedHandler([this]()
            {
                int appviewId = ApplicationView::GetForCurrentView()->Id;
                ApplicationViewSwitcher::TryShowAsStandaloneAsync(
                    appviewId, ViewSizePreference::Default, mainviewId, ViewSizePreference::Default);
            }));
        }
        else
        {
            Frame->Navigate(TypeName(PlaybackPage::typeid), selectedVideoItem->SourceUri);
        }
    }
}

struct VideoItemInfo
{
    const wchar_t* title;
    const wchar_t* source;
};

const VideoItemInfo sampleVideos[] = {
    // You can add more sample videos here.
    {
        L"Sample Video",
        L"https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/SampleVideo.mp4",
    }
};

void _360VideoPlayback::VideoGallery::OnLoaded(Object^ sender, RoutedEventArgs^ e)
{
    auto videoItemList = ref new Vector<Object^>();
    for (auto const& info : sampleVideos)
    {
        auto videoItem = ref new VideoItem(ref new String(info.title), ref new Uri(ref new String(info.source)));
        videoItemList->Append(videoItem);
    }

    this->GalleryGridView->ItemsSource = videoItemList;
}
