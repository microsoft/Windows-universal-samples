//
// Scenario1_ViewSwitch.xaml.cpp
// Implementation of the Scenario1_ViewSwitch class
//

#include "pch.h"
#include "AppView.h"
#include "Scenario1_ViewSwitch.xaml.h"
#include "MainPage.xaml.h"

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml;

namespace SDKTemplate
{
  Scenario1_ViewSwitch::Scenario1_ViewSwitch()
  {
    InitializeComponent();

    try
    {
      // Set up the secondary view, but don't show it yet
      m_exclusiveView = CoreApplication::CreateNewView(ref new SDKTemplate::AppViewSource());
    }
    catch (Platform::COMException^ e)
    {
      // This exception is thrown if the environment doesn't support holographic content
      SwitchButton->IsEnabled = false;
      MainPage::Current->NotifyUser(L"Holographic environment not available.", NotifyType::ErrorMessage);
      return;
    }

    // This is a change from 8.1: In order for the view to be displayed later it needs to be activated
    m_exclusiveView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
      auto viewId = ApplicationView::GetForCurrentView()->Id;

      CoreWindow::GetForCurrentThread()->Activate();
    }));
  }


  void SDKTemplate::Scenario1_ViewSwitch::SwitchButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
  {
    // We must launch the switch from the exclusive view's UI thread so we can access its view ID
    m_exclusiveView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
      auto viewId = ApplicationView::GetForCurrentView()->Id;

      // But the main view must do the actual switching, so we run it from it's thread
      CoreApplication::MainView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([viewId]()
      {
        auto asyncAction = ApplicationViewSwitcher::SwitchAsync(viewId, ApplicationView::GetForCurrentView()->Id);
      }));
    }));
  }
}