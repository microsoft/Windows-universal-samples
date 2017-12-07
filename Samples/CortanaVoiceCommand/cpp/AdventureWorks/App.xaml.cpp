//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "View\TripListView.xaml.h"
#include "View\TripDetails.xaml.h"
#include "ViewModel\TripVoiceCommand.h"

using namespace AdventureWorks;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::VoiceCommands;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Application template is documented at http://go.microsoft.com/fwlink/?LinkId=402347&clcid=0x409

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched by Cortana
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = ref new Frame();

        // Set the default language
        rootFrame->Language = Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        // Register for Back button events.
        SystemNavigationManager::GetForCurrentView()->BackRequested += ref new Windows::Foundation::EventHandler<BackRequestedEventArgs^>(this, &App::OnBackRequested);

        // Register for navigation events so we can update the Back button.
        rootFrame->Navigated += ref new NavigatedEventHandler(this, &App::OnNavigated);

        if (rootFrame->Content == nullptr)
        {
            // When the navigation stack isn't restored navigate to the first page,
            // configuring the new page by passing required information as a navigation
            // parameter
            rootFrame->Navigate(TypeName(TripListView::typeid), e->Arguments);
        }
        // Place the frame in the current Window
        Window::Current->Content = rootFrame;
        // Ensure the current window is active
        Window::Current->Activate();
    }
    else
    {
        if (rootFrame->Content == nullptr)
        {
            // When the navigation stack isn't restored navigate to the first page,
            // configuring the new page by passing required information as a navigation
            // parameter
            rootFrame->Navigate(TypeName(TripListView::typeid), e->Arguments);
        }
        // Ensure the current window is active
        Window::Current->Activate();
    }

    // Install VCD file, and update phrase list.
    create_task(Package::Current->InstalledLocation->GetFileAsync(L"AdventureWorksCommands.xml"))
        .then([this](StorageFile^ vcdStorageFile)
    {
        return VoiceCommandDefinitionManager::InstallCommandDefinitionsFromStorageFileAsync(vcdStorageFile);

    }).then([this](task<void> t)
    {
        try
        {
            t.get();

            ViewModelLocator^ locator = dynamic_cast<ViewModelLocator^>(App::Current->Resources->Lookup("ViewModelLocator"));
            if (locator != nullptr)
            {
                locator->TripViewModel->UpdateDestinationPhraseList();
            }
        }
        catch (Platform::Exception^ ex)
        {
            OutputDebugStringW(L"Failed to install VCD or update phrase list:");
            OutputDebugStringW(ex->ToString()->Data());
        }

    });
}

/// <summary> Extract the semantic interpretation from the voice command. </summary>
/// <param name="tag">The tag to look up</param>
/// <param name="result">The speech reco result provided when the app is activated </summary>
String^ App::SemanticInterpretation(String^ tag, SpeechRecognitionResult^ result)
{
    auto semanticPropertySet = result->SemanticInterpretation->Properties->Lookup(tag);
    if (semanticPropertySet->Size > 0)
    {
        return semanticPropertySet->GetAt(0);
    }
    return nullptr;
}

/// <summary> Handle activation launches. This may occur due to Cortana triggering a VoiceCommand
/// activation, or a protocol activation, depending on the scenario. Foreground activations will use
/// VoiceCommand activation. Background task activations will use protocol activation. 
/// <param name="e">The activation details (voice command or protocol activation details)</param>
void App::OnActivated(IActivatedEventArgs^ e)
{
    TypeName navigationToPageType;
    TripVoiceCommand^ navigationCommand;

    // Handle foreground activation (the "Show trip to <destination>" Cortana command. Background activation
    // will be handled by the VoiceCommandService runtime component, and may result in protocol activation.
    if (e->Kind == ActivationKind::VoiceCommand)
    {
        VoiceCommandActivatedEventArgs^ commandArgs = dynamic_cast<VoiceCommandActivatedEventArgs^>(e);
        if (commandArgs != nullptr)
        {
            SpeechRecognitionResult^ speechRecoResult = commandArgs->Result;

            String^ voiceCommandName = speechRecoResult->RulePath->GetAt(0);
            String^ textSpoken = speechRecoResult->Text;

            String^ commandMode = this->SemanticInterpretation("commandMode", speechRecoResult);
            if (voiceCommandName->Equals(L"showTripToDestination"))
            {
                String^ destination = this->SemanticInterpretation("destination", speechRecoResult);

                navigationCommand = ref new TripVoiceCommand(
                    voiceCommandName,
                    commandMode,
                    textSpoken,
                    destination);

                navigationToPageType = TypeName(TripDetails::typeid);
            }
            else
            {
                navigationToPageType = TypeName(TripListView::typeid);
            }
        }
    }
    else if (e->Kind == ActivationKind::Protocol)
    {
        // Any links back to your app within Cortana will launch it via Protocol Activation.
        ProtocolActivatedEventArgs^ commandArgs = dynamic_cast<ProtocolActivatedEventArgs^>(e);
        if (commandArgs != nullptr)
        {
            WwwFormUrlDecoder^ decoder = ref new WwwFormUrlDecoder(commandArgs->Uri->Query);
            String^ destination = nullptr;
            try
            {
                destination = decoder->GetFirstValueByName("LaunchContext");

                navigationCommand = ref new TripVoiceCommand(
                    "protocolLaunch",
                    "text",
                    "destination",
                    destination);

                navigationToPageType = TypeName(TripDetails::typeid);
            }
            catch (InvalidArgumentException^)
            {
                // Invalid Argument is thrown if we're not launched with the LaunchContext set,
                // so go to the main list of trips instead.
                navigationToPageType = TypeName(TripListView::typeid);
            }
        }
    }

    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = ref new Frame();

        // Set the default language
        rootFrame->Language = Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);
    }

    // When the navigation stack isn't restored navigate to the first page,
    // configuring the new page by passing required information as a navigation
    // parameter
    rootFrame->Navigate(navigationToPageType, navigationCommand);

    // Place the frame in the current Window
    Window::Current->Content = rootFrame;
    // Ensure the current window is active
    Window::Current->Activate();
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

void App::OnNavigated(Object^ sender, NavigationEventArgs^ args)
{
    bool canGoBack = false;
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
    if (rootFrame != nullptr)
    {
        // If we returned to the root page, then empty the backstack.
        if (rootFrame->BackStack->Size > 0 &&
            args->SourcePageType.Name == rootFrame->BackStack->GetAt(0)->SourcePageType.Name)
        {
            rootFrame->BackStack->Clear();
        }

        canGoBack = rootFrame->CanGoBack;
    }

    // Set the Back button state appropriately.
    SystemNavigationManager::GetForCurrentView()->AppViewBackButtonVisibility =
        canGoBack ? AppViewBackButtonVisibility::Visible : AppViewBackButtonVisibility::Collapsed;
}

void App::OnBackRequested(Object^ sender, BackRequestedEventArgs^ e)
{
    if (!e->Handled)
    {
        auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
        if (rootFrame != nullptr && rootFrame->CanGoBack)
        {
            e->Handled = true;
            rootFrame->GoBack();
        }
    }
}
