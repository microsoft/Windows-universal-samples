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
#include "AdventureWorksVoiceCommandService.h"

using namespace VoiceCommandService;

using namespace AdventureWorks_Shared;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::AppService;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::ApplicationModel::VoiceCommands;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Foundation;
using namespace Windows::Storage;

/// <summary>
/// Background task entrypoint. Voice Commands using the <VoiceCommandService Target="...">
/// tag will invoke this when they are recognized by Cortana, passing along details of the 
/// invocation. 
/// 
/// Background tasks must respond to activation by Cortana within 0.5 seconds, and must 
/// report progress to Cortana every 5 seconds (unless Cortana is waiting for user
/// input). There is no execution time limit on the background task managed by Cortana, 
/// which is in contrast to typical background task execution limits.
/// 
/// Typically, Cortana dismisses its UI if it loses focus. This will cause it to terminate 
/// the background task under normal circumstances. However, while debugging, Cortana's UI
/// will not dismiss, and background execution timeouts are suspended, allowing developers 
/// easily debug Cortana's behavior. 
/// In some cases, Remote Debugging may make the process easier (a Virtual Machine and
/// Remote Desktop Connection with remote audio configured for example), but should not
/// be required.
/// </summary>
/// <param name="taskInstance">Connection to the hosting background service process.</param>
void AdventureWorksVoiceCommandService::Run(IBackgroundTaskInstance ^taskInstance)
{
    serviceDeferral = taskInstance->GetDeferral();

    // Register to receive an event if Cortana dismisses the background task. This will
    // occur if the task takes too long to respond, or if Cortana's UI is dismissed.
    // Any pending operations should be cancelled or waited on to clean up where possible.
    taskCancelledToken = taskInstance->Canceled +=
        ref new BackgroundTaskCanceledEventHandler(
            this,
            &AdventureWorksVoiceCommandService::OnCanceled);

    AppServiceTriggerDetails^ triggerDetails = dynamic_cast<AppServiceTriggerDetails^>(taskInstance->TriggerDetails);

    // This should match the uap:AppService and VoiceCommandService references from the 
    // package manifest and VCD files, respectively. Make sure we've been launched by
    // a Cortana Voice Command.
    if (triggerDetails != nullptr && triggerDetails->Name == "AdventureWorksVoiceCommandService")
    {
        try
        {
            voiceServiceConnection = VoiceCommandServiceConnection::FromAppServiceTriggerDetails(triggerDetails);

            voiceServiceConnection->VoiceCommandCompleted +=
                ref new TypedEventHandler<
                VoiceCommandServiceConnection ^,
                VoiceCommandCompletedEventArgs ^>(
                    this,
                    &AdventureWorksVoiceCommandService::OnVoiceCommandCompleted);

            resourceMap = ResourceManager::Current->MainResourceMap->GetSubtree("Resources");
			      // Select the system language, which is what Cortana should be running as.
            context = ResourceContext::GetForViewIndependentUse();

            // GetVoiceCommandAsync establishes initial connection to Cortana, and must be called prior to any 
            // messages sent to Cortana. Attempting to use ReportSuccessAsync, ReportProgressAsync, etc
            // prior to calling this will produce undefined behavior.
            // Note, since this is running synchronously, and not part of the UI thread, blocking
            // with .wait() is ideal in most cases.
            create_task(voiceServiceConnection->GetVoiceCommandAsync())
                .then([this](VoiceCommand^ voiceCommand)
            {
                // Depending on the operation (defined in AdventureWorks:AdventureWorksCommands.xml)
                // perform the appropriate command.
                if (voiceCommand->CommandName == L"whenIsTripToDestination")
                {
                    auto destination = voiceCommand->Properties->Lookup(L"destination")->GetAt(0);
                    this->SendCompletionMessageForDestination(destination);
                }
                else if (voiceCommand->CommandName == L"cancelTripToDestination")
                {
                    auto destination = voiceCommand->Properties->Lookup(L"destination")->GetAt(0);
                    this->SendCompletionMessageForCancellation(destination);
                }
                else
                {
                    // As with app activation VCDs, we need to handle the possibility that
                    // an app update may remove a voice command that is still registered.
                    // This can happen if the user hasn't run an app since an update.
                    this->LaunchAppInForeground();
                }
            }).wait();
        }
        catch (Exception^ ex)
        {
            OutputDebugStringW(ex->ToString()->Data());
        }
    }
}

/// <summary>
/// When the background task is cancelled, clean up/cancel any ongoing long-running operations.
/// This cancellation notice may not be due to Cortana directly. The voice command connection will
/// typically already be destroyed by this point and should not be expected to be active.
/// </summary>
/// <param name="sender">This background task instance</param>
/// <param name="reason">Contains an enumeration with the reason for task cancellation</param>
void AdventureWorksVoiceCommandService::OnCanceled(IBackgroundTaskInstance ^sender, BackgroundTaskCancellationReason reason)
{
    OutputDebugStringW(L"Task cancelled, cleaning up");
    if (this->serviceDeferral.Get() != nullptr)
    {
        this->serviceDeferral->Complete();
    }
}

/// <summary>
/// Handle the completion of the voice command. Your app may be cancelled
/// for a variety of reasons, such as user cancellation or not providing 
/// progress to Cortana in a timely fashion. Clean up any pending long-running
/// operations (eg, network requests).
/// </summary>
/// <param name="sender">The voice connection associated with the command.</param>
/// <param name="args">Contains an Enumeration indicating why the command was terminated.</param>
void AdventureWorksVoiceCommandService::OnVoiceCommandCompleted(VoiceCommandServiceConnection ^sender, VoiceCommandCompletedEventArgs ^args)
{
    if (this->serviceDeferral.Get() != nullptr)
    {
        this->serviceDeferral->Complete();
    }
}

/// <summary>
/// Handle the Trip Cancellation task. This task demonstrates how to prompt a user
/// for confirmation of an operation, show users a progress screen while performing
/// a long-running task, and showing a completion screen.
/// </summary>
/// <param name="destination">The name of a destination, expected to match the phrase list.</param>
/// <returns></returns>
void AdventureWorksVoiceCommandService::SendCompletionMessageForDestination(String^ destination)
{
    // Begin loading data to search for the target trip. If this operation is going to take 
    // a non-trivial amount of time, show a progress screen in order to prevent Cortana
    // from timing this background operation out.
    String^ progressMessage = GetResourceStringAndFormat(L"LoadingTripToDestination", destination);
    this->ShowProgressScreen(progressMessage);

    TripStore^ store = ref new TripStore();
    store->LoadTripsSync();

    std::vector<Trip^> trips;
    for (unsigned int i = 0; i < store->Trips->Size; i++)
    {
        if (store->Trips->GetAt(i)->Destination == destination)
        {
            trips.push_back(store->Trips->GetAt(i));
        }
    }

    VoiceCommandUserMessage^ userMessage = ref new VoiceCommandUserMessage();
    Vector<VoiceCommandContentTile^>^ destinationContentTiles = ref new Vector<VoiceCommandContentTile^>();

    if (trips.size() == 0)
    {
        String^ foundNoTripToDestination = GetResourceStringAndFormat(L"NoSuchTripToDestination", destination);
        userMessage->DisplayMessage = foundNoTripToDestination;
        userMessage->SpokenMessage = foundNoTripToDestination;
    }
    else
    {
        String^ message = "";
        if (trips.size() > 1)
        {
			message = GetResourceString(L"PluralUpcomingTrips"); 
        }
        else
        {
			message = GetResourceString(L"SingularUpcomingTrip"); 
        }

        userMessage->DisplayMessage = message;
        userMessage->SpokenMessage = message;

        std::for_each(begin(trips), end(trips), [destinationContentTiles, destination](Trip^ trip) {
            int i = 0;

            auto destinationTile = ref new VoiceCommandContentTile();
            destinationTile->ContentTileType = VoiceCommandContentTileType::TitleWith68x68IconAndText;

            create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Uri(L"ms-appx:///VoiceCommandService/Images/GreyTile.png")))
                .then([destinationTile](StorageFile^ imageFileRef)
            {
                destinationTile->Image = imageFileRef;
            }).wait();

            destinationTile->AppLaunchArgument = destination;
            destinationTile->Title = trip->Destination;

            if (trip->StartDate != nullptr)
            {
                auto formatter = ref new DateTimeFormatter("longdate");

                destinationTile->TextLine1 = formatter->Format(trip->StartDate->GetDateTime());
            }
            else
            {
                destinationTile->TextLine1 = trip->Destination + L" " + i.ToString();
            }

            destinationContentTiles->Append(destinationTile);
            i++;
        });
    }

    auto response = VoiceCommandResponse::CreateResponse(userMessage, destinationContentTiles);

    if (trips.size() > 0)
    {
        response->AppLaunchArgument = destination;
    }

    create_task(voiceServiceConnection->ReportSuccessAsync(response)).wait();
}

/// <summary>
/// Handle the Trip Cancellation task. This task demonstrates how to prompt a user
/// for confirmation of an operation, show users a progress screen while performing
/// a long-running task, and showing a completion screen.
/// </summary>
/// <param name="destination">The name of a destination, expected to match the phrase list.</param>
/// <returns></returns>
void AdventureWorksVoiceCommandService::SendCompletionMessageForCancellation(String^ destination)
{
    // Begin loading data to search for the target trip. If this operation is going to take 
    // a non-trivial amount of time, show a progress screen in order to prevent Cortana
    // from timing this background operation out.
    String^ progressMessage = GetResourceStringAndFormat(L"LoadingTripToDestination", destination);
    this->ShowProgressScreen(progressMessage);

    TripStore^ store = ref new TripStore();
    store->LoadTripsSync();

    std::vector<Trip^> trips;
    for (unsigned int i = 0; i < store->Trips->Size; i++)
    {
        if (store->Trips->GetAt(i)->Destination == destination)
        {
            trips.push_back(store->Trips->GetAt(i));
        }
    }

    Trip^ trip = nullptr;
    if (trips.size() > 1)
    {
        // If we have more than one trip, provide a disambiguation screen rather than just picking one
        // or deleting all of them. If there's more than three items, you may just decide to show a 
        // link to a deeper search in your app's experience.
        String^ disambiguationMessage = GetResourceStringAndFormat(L"DisambiguationWhichTripToDest", destination);
		String^ secondDisambiguationMessage = GetResourceString(L"DisambiguationRepeat");
        trip = DisambiguateTrips(trips, disambiguationMessage, secondDisambiguationMessage);
    }
    else if (trips.size() == 1)
    {
        trip = trips.at(0);
    }

    if (trip == nullptr)
    {
        auto userMessage = ref new VoiceCommandUserMessage();

        userMessage->DisplayMessage = GetResourceStringAndFormat(L"FoundNoTripToDestination", destination);

        auto response = VoiceCommandResponse::CreateResponse(userMessage);
        create_task(voiceServiceConnection->ReportFailureAsync(response)).wait();
    }
    else
    {
        String^ cancelTripToDestination = GetResourceStringAndFormat(L"CancelTripToDestination", destination); 
        auto userPrompt = ref new VoiceCommandUserMessage();

        userPrompt->DisplayMessage = cancelTripToDestination;
        userPrompt->SpokenMessage = cancelTripToDestination;

        auto userReprompt = ref new VoiceCommandUserMessage();
        String^ repromptMessage = GetResourceStringAndFormat(L"ConfirmCancelTripToDestination", destination);
        userReprompt->DisplayMessage = repromptMessage;
        userReprompt->SpokenMessage = repromptMessage;

        auto response = VoiceCommandResponse::CreateResponseForPrompt(userPrompt, userReprompt);

        VoiceCommandConfirmationResult^ voiceCommandConfirmation;

        create_task(voiceServiceConnection->RequestConfirmationAsync(response))
            .then([&voiceCommandConfirmation](VoiceCommandConfirmationResult^ confirmation)
        {
            voiceCommandConfirmation = confirmation;
        }).wait();

        // If the result of the Confirmation is null, likely Cortana was dismissed without giving a response. 
        if (voiceCommandConfirmation != nullptr)
        {
            if (voiceCommandConfirmation->Confirmed)
            {
                String^ cancellingTripToDestination = GetResourceStringAndFormat(L"CancellingTripToDestination", destination);
                ShowProgressScreen(cancellingTripToDestination);

                create_task(store->DeleteTripAsync(trip)).wait();

                auto userMessage = ref new VoiceCommandUserMessage();
                String^ successMessage = GetResourceStringAndFormat(L"CancelledTripToDestination", destination);
                userMessage->DisplayMessage = successMessage;
                userMessage->SpokenMessage = successMessage;

                auto response = VoiceCommandResponse::CreateResponse(userMessage);
                create_task(voiceServiceConnection->ReportSuccessAsync(response)).wait();
            }
            else
            {
                auto userMessage = ref new VoiceCommandUserMessage();
                String^ cancelMessage = GetResourceStringAndFormat(L"KeepingTripToDestination", destination);
                userMessage->DisplayMessage = cancelMessage;
                userMessage->SpokenMessage = cancelMessage;

                auto response = VoiceCommandResponse::CreateResponse(userMessage);
                create_task(voiceServiceConnection->ReportSuccessAsync(response)).wait();
            }
        }
    }
}

/// <summary>
/// Demonstrates providing the user with a choice between multiple items. In this case, if a user
/// has two trips to the same destination with different dates, this will provide a way to differentiate
/// them. Provide a way to choose between the items.
/// </summary>
/// <param name="trips">The set of trips to choose between</param>
/// <param name="disambiguationMessage">The initial disambiguation message</param>
/// <param name="secondDisambiguationMessage">Repeat prompt retry message</param>
/// <returns></returns>
Trip^ VoiceCommandService::AdventureWorksVoiceCommandService::DisambiguateTrips(std::vector<Trip^> trips, Platform::String ^ disambiguationMessage, String ^ secondDisambiguationMessage)
{
    auto userPrompt = ref new VoiceCommandUserMessage();
    userPrompt->DisplayMessage = disambiguationMessage;
    userPrompt->SpokenMessage = disambiguationMessage;

    auto userReprompt = ref new VoiceCommandUserMessage();
    userReprompt->DisplayMessage = secondDisambiguationMessage;
    userReprompt->SpokenMessage = secondDisambiguationMessage;

    Vector<VoiceCommandContentTile^>^ destinationContentTiles = ref new Vector<VoiceCommandContentTile^>();
    int i = 1;
    std::for_each(begin(trips), end(trips), [destinationContentTiles, &i](Trip^ trip) {
        auto destinationTile = ref new VoiceCommandContentTile();

        destinationTile->ContentTileType = VoiceCommandContentTileType::TitleWith68x68IconAndText;
        create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Uri(L"ms-appx:///VoiceCommandService/Images/GreyTile.png")))
            .then([destinationTile](StorageFile^ imageFileRef)
        {
            destinationTile->Image = imageFileRef;
        }).wait();

        destinationTile->AppContext = trip;
        String^ dateFormat;
        if (trip->StartDate != nullptr)
        {
            auto formatter = ref new DateTimeFormatter("longdate");

            dateFormat = formatter->Format(trip->StartDate->GetDateTime());
        }
        else
        {
            dateFormat = i.ToString();
        }

        destinationTile->Title = trip->Destination + L" " + dateFormat;
        destinationTile->TextLine1 = trip->Description;

        destinationContentTiles->Append(destinationTile);
        i++;
    });

    auto response = VoiceCommandResponse::CreateResponseForPrompt(userPrompt, userReprompt, destinationContentTiles);

    Trip^ selectedTrip = nullptr;
    create_task(voiceServiceConnection->RequestDisambiguationAsync(response))
        .then([&selectedTrip](VoiceCommandDisambiguationResult^ result) {
        if (result != nullptr)
        {
            selectedTrip = dynamic_cast<Trip^>(result->SelectedItem->AppContext);
        }
    }).wait();
    return selectedTrip;
}

/// <summary>
/// Provide a simple response that launches the app. Expected to be used in the
/// case where the voice command could not be recognized (eg, a VCD/code mismatch.)
/// </summary>
void VoiceCommandService::AdventureWorksVoiceCommandService::LaunchAppInForeground()
{
    auto userProgressMessage = ref new VoiceCommandUserMessage();
	userProgressMessage->SpokenMessage = GetResourceString(L"LaunchingAdventureWorks");

    VoiceCommandResponse^ response = VoiceCommandResponse::CreateResponse(userProgressMessage);
    response->AppLaunchArgument = L"";
    create_task(voiceServiceConnection->ReportProgressAsync(response));
}

/// <summary>
/// Show a progress screen. These should be posted at least every 5 seconds for a 
/// long-running operation, such as accessing network resources over a mobile 
/// carrier network.
/// </summary>
/// <param name="message">The message to display, relating to the task being performed.</param>
/// <returns></returns>
void VoiceCommandService::AdventureWorksVoiceCommandService::ShowProgressScreen(Platform::String ^ progressMessage)
{
    auto userProgressMessage = ref new VoiceCommandUserMessage();
    userProgressMessage->DisplayMessage = progressMessage;
    userProgressMessage->SpokenMessage = progressMessage;

    VoiceCommandResponse^ response = VoiceCommandResponse::CreateResponse(userProgressMessage);
    create_task(voiceServiceConnection->ReportProgressAsync(response));
}

/// <Summary> Look up a resource string and return it. </Summary>
/// <returns>The resource string requested for the current context</returns>
String ^ AdventureWorksVoiceCommandService::GetResourceString(String ^ resourceName)
{
	return resourceMap->GetValue(resourceName, context)->ValueAsString;
}

/// <Summary> Look up a resource string and replace {0} with the parameter provided (simple string.format) </Summary>
/// <returns>The resource string requested for the current context, with substitution</returns>
String ^ AdventureWorksVoiceCommandService::GetResourceStringAndFormat(String ^ resourceName, Platform::String ^ param)
{
	String^ resource = GetResourceString(resourceName);
	std::wstring str = resource->Data();
	std::wstring to = param->Data();
	std::wstring positionText = L"{0}";
	size_t pos = str.find(positionText, 0);
	if (pos != std::string::npos)
	{
		str.replace(pos, positionText.length(), to);
	}

	String^ outString = ref new String(str.data());
	return outString;
}
