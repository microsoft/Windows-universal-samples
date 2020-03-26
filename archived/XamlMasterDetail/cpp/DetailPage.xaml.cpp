//
// DetailPage.xaml.cpp
// Implementation of the DetailPage class
//

#include "pch.h"
#include "DetailPage.xaml.h"

using namespace MasterDetailApp;
using namespace MasterDetailApp::Data;
using namespace MasterDetailApp::ViewModels;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

DetailPage::DetailPage()
    : m_backRequestedEventRegistrationToken(), m_sizeChangedEventRegistrationToken()
{
    InitializeComponent();
}

// See https://msdn.microsoft.com/en-us/library/hh920267.aspx
DependencyProperty^ DetailPage::s_itemProperty = nullptr;

void DetailPage::RegisterDependencyProperties()
{
    s_itemProperty =
        DependencyProperty::Register(
            L"Item",
            TypeName(ItemViewModel::typeid),
            TypeName(DetailPage::typeid),
            ref new PropertyMetadata(nullptr)
            );
}

void DetailPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    // Parameter is item ID
    Item = ItemViewModel::FromItem(ItemsDataSource::GetItemById(safe_cast<int>(e->Parameter)));

    auto backStack = Frame->BackStack;
    auto backStackCount = backStack->Size;

    if (backStackCount > 0)
    {
        auto masterPageEntry = backStack->GetAt(backStackCount - 1);
        backStack->RemoveAt(backStackCount - 1);

        // Doctor the navigation parameter for the master page so it
        // will show the correct item in the side-by-side view.
        auto modifiedEntry = ref new PageStackEntry(
            masterPageEntry->SourcePageType,
            Item->ItemId,
            masterPageEntry->NavigationTransitionInfo
            );
        backStack->Append(modifiedEntry);
    }

    // Register for hardware and software back request from the system
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    m_backRequestedEventRegistrationToken =
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &DetailPage::DetailPage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

void DetailPage::OnNavigatedFrom(NavigationEventArgs ^ e)
{
    Page::OnNavigatedFrom(e);

    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    systemNavigationManager->BackRequested -= m_backRequestedEventRegistrationToken;
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Collapsed;
}

void DetailPage::PageRoot_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    if (ShouldGoToWideState())
    {
        // We shouldn't see this page since we are in "wide master-detail" mode.
        // Play a transition as we are likely navigating back from a separate page.
        NavigateBackForWideState(/* useTransition */ true);
    }
    else
    {
        // Realize the main page content.
        FindName("RootPanel");
    }

    m_sizeChangedEventRegistrationToken =
        Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &DetailPage::Window_SizeChanged);
}


void DetailPage::PageRoot_Unloaded(Object^ sender, RoutedEventArgs^ e)
{
    Window::Current->SizeChanged -= m_sizeChangedEventRegistrationToken;
}

void MasterDetailApp::DetailPage::BackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OnBackRequested();
}

void DetailPage::Window_SizeChanged(Object ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e)
{
    if (ShouldGoToWideState())
    {
        // Make sure we are no longer listening to window change events.
        Window::Current->SizeChanged -= m_sizeChangedEventRegistrationToken;

        // We shouldn't see this page since we are in "wide master-detail" mode.
        NavigateBackForWideState(/* useTransition */ false);
    }
}


void DetailPage::DetailPage_BackRequested(Object ^sender, BackRequestedEventArgs ^args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    OnBackRequested();
}

void DetailPage::OnBackRequested()
{
    // Page above us will be our master view.
    // Make sure we are using the "drill out" animation in this transition.

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

void DetailPage::NavigateBackForWideState(bool useTransition)
{
    // Evict this page from the cache as we may not need it again.
    NavigationCacheMode = Windows::UI::Xaml::Navigation::NavigationCacheMode::Disabled;

    if (useTransition)
    {
        Frame->GoBack(ref new EntranceNavigationTransitionInfo());
    }
    else
    {
        Frame->GoBack(ref new SuppressNavigationTransitionInfo());
    }
}

bool DetailPage::ShouldGoToWideState()
{
    return Window::Current->Bounds.Width >= 720.0;
}