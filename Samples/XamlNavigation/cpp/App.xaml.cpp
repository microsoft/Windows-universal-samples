//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "AppShell.xaml.h"
#include "LandingPage.xaml.h"
#include "SystemInformationHelpers.h"

using namespace NavigationMenuSample;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{

#if _DEBUG
    // Show graphics profiling information while debugging.
    if (IsDebuggerPresent())
    {
        // Display the current frame rate counters
         //DebugSettings->EnableFrameRateCounter = true;
    }
#endif

    // Change minimum window size
    ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(320, 200));

    // Darken the window title bar using a color value to match app theme
    ApplicationViewTitleBar^ titleBar = ApplicationView::GetForCurrentView()->TitleBar;
    if (titleBar != nullptr)
    {
        Color color = safe_cast<Color>(this->Resources->Lookup(L"SystemChromeMediumColor"));
        titleBar->BackgroundColor = color;
        titleBar->ButtonBackgroundColor = color;
    }

    if (SystemInformationHelpers::IsTenFootExperience())
    {
        // Apply guidance from https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv
        ApplicationView::GetForCurrentView()->SetDesiredBoundsMode(ApplicationViewBoundsMode::UseCoreWindow);

        ResourceDictionary^ TenFootDictionary = ref new ResourceDictionary();
        TenFootDictionary->Source = ref new Uri("ms-appx:///Styles/TenFootStylesheet.xaml");
        this->Resources->MergedDictionaries->Append(TenFootDictionary);
    }

    auto shell = dynamic_cast<AppShell^>(Window::Current->Content);

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (shell == nullptr)
    {
        // Create a AppShell to act as the navigation context and navigate to the first page
        shell = ref new AppShell();

        shell->AppFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
            // TODO: Restore the saved session state only when appropriate, scheduling the
            // final launch steps after the restore is complete

        }
    }

    // Place our app shell in the current Window
    Window::Current->Content = shell;

    if (shell->AppFrame->Content == nullptr)
    {
        // When the navigation stack isn't restored navigate to the first page,
        // suppressing the initial entrance animation and configuring the new
        // page by passing required information as a navigation parameter
        shell->AppFrame->Navigate(TypeName(Views::LandingPage::typeid), e->Arguments, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo());
    }

    // Ensure the current window is active
    Window::Current->Activate();
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void)sender;   // Unused parameter
    (void)e;    // Unused parameter

    //TODO: Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}