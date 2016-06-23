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

var appData = Windows.Storage.ApplicationData;
var voiceCommands = Windows.ApplicationModel.VoiceCommands;
var appService = Windows.ApplicationModel.AppService;

(function () {
    "use strict";

    // Get the background task details
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;
    var voiceServiceConnection = null;

    // implement simple string.format() search/replace for use with localized strings.
    if (!String.prototype.format) {
        String.prototype.format = function () {
            var args = arguments;
            return this.replace(/{(\d+)}/g, function (match, number) {
                return typeof args[number] != 'undefined'
                  ? args[number]
                  : match
                ;
            });
        };
    }

    // localization resources
    var context;
    var resourceMap;
    var dateFormatter;

    function doWork() {
        /// <summary>Main entrypoint for background task launched by Cortana. Encapsulated
        /// into a function doWork to break up the work into functional components. Executed with a
        /// call to doWork() below</summary>

        var details = backgroundTaskInstance.triggerDetails;

        var deferral = backgroundTaskInstance.getDeferral();

        // initialize resource lookup.
        var rcns = Windows.ApplicationModel.Resources.Core;
        resourceMap = rcns.ResourceManager.current.mainResourceMap.getSubtree('Resources');
        context = rcns.ResourceContext.getForViewIndependentUse();
        dateFormatter = Windows.Globalization.DateTimeFormatting.DateTimeFormatter("longdate");

        function onCanceled(cancelEventArg) {
            var cancelReason = cancelEventArg;
        }
        backgroundTaskInstance.addEventListener("canceled", onCanceled);

        function onVoiceCommandCompleted(completedEventArg) {
            var completionReason = completedEventArg.completionReason;
        }

        if (details instanceof appService.AppServiceTriggerDetails) {
            voiceServiceConnection = voiceCommands.VoiceCommandServiceConnection.fromAppServiceTriggerDetails(details);
            voiceServiceConnection.addEventListener("voiceCommandCompleted", onVoiceCommandCompleted);

            // getVoiceCommandAsync establishes initial connection to Cortana, and must be called prior to any 
            // messages sent to Cortana. Attempting to use reportSuccessAsync, reportProgressAsync, etc
            // prior to calling this will produce undefined behavior.
            voiceServiceConnection.getVoiceCommandAsync().then(function completed(voiceCommand) {

                switch (voiceCommand.commandName) {
                    case "whenIsTripToDestination":
                        var destination = voiceCommand.properties["destination"][0];
                        showTrips(destination);
                        break;

                    case "cancelTripToDestination":
                        var destination = voiceCommand.properties["destination"][0];
                        cancelTrip(destination);
                        break;

                    default:
                        launchAppInForeground();
                        break;
                }
            });
        }
    }

    function launchAppInForeground() {
        /// <summary> Fall back on launching the app if we can't satisify the request 
        /// (eg, invalid voice command from old VCD) </summary>
        var userMessage = new voiceCommands.VoiceCommandUserMessage();

        var launchingAdventureWorksMessage = resourceMap.getValue("LaunchingAdventureWorks", context).valueAsString;
        userMessage.spokenMessage = launchingAdventureWorksMessage;
        userMessage.displayMessage = launchingAdventureWorksMessage;

        var response = voiceCommands.VoiceCommandResponse.createResponse(userMessage);

        response.appLaunchArgument = "";

        voiceServiceConnection.requestAppLaunchAsync(response).done();
    }


    function disambiguateTrips(trips, disambiguationMessage, repeatDisambiguationMessage) {
        /// <summary> Display a disambiguation flow to users. Loops through the set of trips,
        /// builds content tiles for each, and prompts the user </summary>
        /// <param type="array" name="trips">The set of trips to disambiguate.</param>
        /// <param type="string" name="disambiguationMessage">The initial disambiguation message</param>
        /// <param type="string" name="repeatDisambiguationMessage">The repeated disambiguation message</param>

        // Note: If the content tiles are coming from an internet connection, joining 
        // individual tile image requests together with WinJS.promise.join() will 
        // achieve this.
        var imageUri = new Windows.Foundation.Uri("ms-appx:///images/GreyTile.png");
        var image = null;
        return Windows.Storage.StorageFile.getFileFromApplicationUriAsync(imageUri)
            .then(function (imageFile) {
                image = imageFile;

                var userPrompt = new voiceCommands.VoiceCommandUserMessage();
                userPrompt.displayMessage = disambiguationMessage;
                userPrompt.spokenMessage = disambiguationMessage;

                var userReprompt = new voiceCommands.VoiceCommandUserMessage();
                userReprompt.displayMessage = repeatDisambiguationMessage;
                userReprompt.spokenMessage = repeatDisambiguationMessage;

                var destinationContentTiles = [];
                for (var i = 0; i < trips.length; i++) {
                    var trip = trips[i];

                    var destinationTile = new voiceCommands.VoiceCommandContentTile();
                    destinationTile.contentTileType = voiceCommands.VoiceCommandContentTileType.titleWith68x68IconAndText;
                    destinationTile.image = image;

                    // appContext requires a sealed class or well known type. use the array index or some other
                    // simple unique identifier (the trips array will stay in scope during the disambiguation call)
                    destinationTile.appContext = i;

                    destinationTile.appLaunchArgument = trip.destination;
                    destinationTile.title = trip.destination;
                    if (new Date(trip.startDate) === "Invalid Date" || isNaN(new Date(trip.startDate))) {
                        destinationTile.textLine1 = trip.destination + " " + (i + 1);
                    } else {
                        var dateObj = new Date(trip.startDate);
                        destinationTile.textLine1 = dateFormatter.format(dateObj);
                    }

                    destinationContentTiles.push(destinationTile);
                }

                // Cortana will handle re-prompting until either the user cancels the session, or
                // the user gives a response.
                var response = voiceCommands.VoiceCommandResponse.createResponseForPrompt(
                    userPrompt,
                    userReprompt,
                    destinationContentTiles);

                return voiceServiceConnection.requestDisambiguationAsync(response);
            }).then(function (voiceCommandDisambiguationResult) {
                if (voiceCommandDisambiguationResult != null) {
                    var selectedTile = voiceCommandDisambiguationResult.selectedItem;

                    return trips[selectedTile.appContext];
                }

                return null;
            });
    }

    function cancelTrip(destination) {
        /// <summary>Look for the requested trip, disambiguate if there's more than one trip to that 
        /// destination, and then prompt for confirmation. Upon confirmation, delete the trip and 
        /// write the changes back to disk.</summary>
        /// <param type="string" name="destination">The destination of the trip to cancel.</param>

        var installFolder = Windows.ApplicationModel.Package.current.installedLocation;
        var image = null;
        // the set of all trips in trips.json
        var trips = null;
        // the selected trip, post-disambiguation, if necessary.
        var tripToDelete = null;

        //// display a progress screen while we load files from disk. This gives us around
        //// 5 seconds to respond, in case disk access is slow, or if we're waiting for a 
        //// network response, etc.
        var loadingProgress = resourceMap.getValue("ProgressLookingForTripToDest", context).valueAsString;
        showProgressScreen(loadingProgress.format(destination)).then(function () {
            var imageUri = new Windows.Foundation.Uri("ms-appx:///images/GreyTile.png");
            return Windows.Storage.StorageFile.getFileFromApplicationUriAsync(imageUri);
        }).then(function (imageFile) {
            // save this to a variable that will be accessible for the entire scope of this process.;
            image = imageFile;
            return loadTripData();
        }).then(function (tripSet) {
            trips = tripSet;
            var foundTrips = [];
            trips.forEach(function (trip) {
                if (trip.destination == destination) {
                    foundTrips.push(trip);
                }
            });

            // Three situations. No trips exist, One trip exists, or multiple trips exist.
            if (foundTrips.length == 0) {
                return null;
            } else if (foundTrips.length == 1) {
                return foundTrips[0];
            } else {
                var disambiguationString = resourceMap.getValue("DisambiguationWhichTripToDest", context).valueAsString;
                var repeatDisambiguationString = resourceMap.getValue("DisambiguationRepeat", context).valueAsString;

                return disambiguateTrips(
                    foundTrips,
                    disambiguationString.format(destination),
                    repeatDisambiguationString);
            }
        }).then(function (trip) {
            tripToDelete = trip;

            var response = null;
            if (tripToDelete == null) {
                var userMessage = new voiceCommands.VoiceCommandUserMessage();
                var foundNoTrips = resourceMap.getValue("FoundNoTripToDestination", context).valueAsString;
                userMessage.displayMessage = foundNoTrips.format(destination);
                userMessage.spokenMessage = foundNoTrips.format(destination);
                response = voiceCommands.VoiceCommandResponse.createResponse(userMessage);
                return voiceServiceConnection.reportSuccessAsync(response);
            } else {
                var userPrompt = new voiceCommands.VoiceCommandUserMessage();
                // confirmation prompt.
                var cancelTrip = resourceMap.getValue("CancelTripToDestination", context).valueAsString;
                userPrompt.displayMessage = cancelTrip.format(destination);
                userPrompt.spokenMessage = cancelTrip.format(destination);

                // If the user doesn't respond in a meaningful way, or Cortana doesn't understand them,
                // a reprompt message will be shown instead. This can be an abbreviated message, or 
                // contain a longer message with more specific information, etc.
                var userReprompt = new voiceCommands.VoiceCommandUserMessage();
                var confirmCancelTrip = resourceMap.getValue("ConfirmCancelTripToDestination", context).valueAsString;
                userReprompt.displayMessage = confirmCancelTrip.format(destination);
                userReprompt.spokenMessage = confirmCancelTrip.format(destination);

                response = voiceCommands.VoiceCommandResponse.createResponseForPrompt(
                    userPrompt,
                    userReprompt);

                return voiceServiceConnection.requestConfirmationAsync(response);

            }

        }).then(function (voiceCommandConfirmation) {
            // if we displayed a "no such trip", this should be null. This could also happen
            // if Cortana is dismissed instead of answering the question.
            if (voiceCommandConfirmation != null) {
                if (voiceCommandConfirmation.confirmed == true) {
                    var cancelProgress = resourceMap.getValue("CancellingTripToDestination", context).valueAsString;

                    return showProgressScreen(cancelProgress.format(destination)).
                        then(function () {
                            // remove the trip, start saving.
                            var index = trips.indexOf(tripToDelete);
                            if (index != -1) {
                                trips.splice(index, 1);
                            }
                            return saveTripsToFile(trips);
                        }).then(function () {
                            var completedMessage = new voiceCommands.VoiceCommandUserMessage();
                            var cancelComplete = resourceMap.getValue("CancelledTripToDestination", context).valueAsString;
                            completedMessage.displayMessage = cancelComplete.format(destination);
                            completedMessage.spokenMessage = cancelComplete.format(destination);

                            var response = voiceCommands.VoiceCommandResponse.createResponse(completedMessage);
                            return voiceServiceConnection.reportSuccessAsync(response);
                        });
                } else {
                    // User declined, keep the trip and send a completion message affirming this
                    // choice
                    var completedMessage = new voiceCommands.VoiceCommandUserMessage();
                    var keepingTrip = resourceMap.getValue("KeepingTripToDestination", context).valueAsString;
                    completedMessage.displayMessage = keepingTrip.format(destination);
                    completedMessage.spokenMessage = keepingTrip.format(destination);

                    var response = voiceCommands.VoiceCommandResponse.createResponse(completedMessage);
                    return voiceServiceConnection.reportSuccessAsync(response);
                }
            }
        }).done();
    }

    function showProgressScreen(progressMessage) {
        /// <summary>Send a message to Cortana to show as a progress screen. This gives a background
        /// task 5 seconds before Cortana will time it out. Progress screens can be re-sent periodically
        /// to handle long-running operations (such as lengthy network queries with multiple query/responses)
        /// </summary>
        /// <param type="string" name="progressMessage">The message to display.</param>

        var userProgressMessage = new voiceCommands.VoiceCommandUserMessage();

        userProgressMessage.displayMessage = progressMessage;
        userProgressMessage.spokenMessage = progressMessage;

        var response = voiceCommands.VoiceCommandResponse.createResponse(userProgressMessage);
        return voiceServiceConnection.reportProgressAsync(response);
    }

    function showTrips(destination) {
        /// <summary>Load the trip set from disk, look for destinations matching, and send a
        /// response set that shows several trips. Ideally, this should limit to 2-3 responses
        /// <param type="string" name="destination">The destination to search for.</param>
        var installFolder = Windows.ApplicationModel.Package.current.installedLocation;
        var image = null;

        //// display a progress screen while we load files from disk. This gives us around
        //// 5 seconds to respond, in case disk access is slow, or if we're waiting for a 
        //// network response, etc.
        var lookingForTrip = resourceMap.getValue("LoadingTripToDestination", context).valueAsString;
        showProgressScreen(lookingForTrip.format(destination)).then(function () {
            var imageUri = new Windows.Foundation.Uri("ms-appx:///images/GreyTile.png");
            return Windows.Storage.StorageFile.getFileFromApplicationUriAsync(imageUri);
        }).then(function (imageFile) {
            image = imageFile;

            return loadTripData();
        }).then(function (trips) {
            var foundTrips = [];
            trips.forEach(function (candidateTrip) {
                if (candidateTrip.destination == destination) {
                    foundTrips.push(candidateTrip);
                }
            });

            var userMessage = new voiceCommands.VoiceCommandUserMessage();
            if (foundTrips.length == 0) {
                var noTripFound = resourceMap.getValue("NoSuchTripToDestination", context).valueAsString;

                userMessage.displayMessage = noTripFound.format(destination);
                userMessage.spokenMessage = noTripFound.format(destination);
            } else {
                if (foundTrips.length == 1) {
                    // singular message
                    var foundTripSingular = resourceMap.getValue("SingularUpcomingTrip", context).valueAsString;
                    userMessage.displayMessage = foundTripSingular;
                    userMessage.spokenMessage = foundTripSingular;
                } else {
                    // plural message
                    var foundTripsPlural = resourceMap.getValue("PluralUpcomingTrips", context).valueAsString;

                    userMessage.displayMessage = foundTripsPlural;
                    userMessage.spokenMessage = foundTripsPlural;
                }
                var destinationContentTiles = [];
                for (var i = 0; i < foundTrips.length; i++) {
                    var trip = foundTrips[i];
                    var destinationTile = new voiceCommands.VoiceCommandContentTile();

                    destinationTile.contentTileType = voiceCommands.VoiceCommandContentTileType.titleWith68x68IconAndText;
                    destinationTile.image = image;

                    // if there's multiple items, ideally you would provide a unique ID.
                    // for the sake of brevity, this sample does not.
                    destinationTile.appLaunchArgument = destination;
                    destinationTile.title = trip.destination;
                    if (new Date(trip.startDate) === "Invalid Date" || isNaN(new Date(trip.startDate))) {
                        destinationTile.textLine1 = trip.destination + " " + (i + 1);
                    } else {
                        var dateObj = new Date(trip.startDate);
                        destinationTile.textLine1 = dateFormatter.format(dateObj);
                    }

                    destinationContentTiles.push(destinationTile);
                }

                var response = voiceCommands.VoiceCommandResponse.createResponse(userMessage, destinationContentTiles);

                if (foundTrips.length > 0) {
                    response.appLaunchArgument = destination;
                }

                return voiceServiceConnection.reportSuccessAsync(response);
            }

        }).done();
    }

    function loadTripData() {
        return appData.current.localFolder.tryGetItemAsync("trips.json").then(
            function (file) {
                if (file == null) {
                    /// no trips available, return an empty set.
                    return {};
                } else {
                    return Windows.Storage.FileIO.readTextAsync(file).then(function (text) {
                        var trips = JSON.parse(text);
                        return trips;
                    });
                }
            });
    }

    function saveTripsToFile(trips) {
        return appData.current.localFolder.createFileAsync("trips.json", Windows.Storage.CreationCollisionOption.replaceExisting)
            .then(function (outputFile) {
                var outText = JSON.stringify(trips.slice(0));
                return Windows.Storage.FileIO.writeTextAsync(outputFile, outText);
            });
    }

    // Start processing the background task.
    doWork();

})();