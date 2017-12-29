//
// MainPage.xaml.cpp
// Implementation of the MainPage class
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "VideoGallery.xaml.h"

using namespace _360VideoPlayback;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Interop;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
    InitializeComponent();
    SystemNavigationManager::GetForCurrentView()->BackRequested += ref new EventHandler<BackRequestedEventArgs^>(this, &MainPage::OnBackRequested);
    this->contentFrame->Navigate(TypeName(VideoGallery::typeid));
}

void MainPage::OnBackRequested(Object^ sender, BackRequestedEventArgs^ args)
{
    if (this->contentFrame->CanGoBack)
    {
        this->contentFrame->GoBack();
    }
}
