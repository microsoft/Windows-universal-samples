#include "pch.h"
#include "RandomNumberGeneratorTask.h"
#include <random>

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::AppService;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace RandomNumberService
{
    Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral^> _serviceDeferral;
    AppServiceConnection^ _connection;
    std::mt19937 _randomNumberGenerator;
    void OnRequestReceived(AppServiceConnection^ connection, AppServiceRequestReceivedEventArgs^ args);

    void RandomNumberGeneratorTask::Run(IBackgroundTaskInstance^ taskInstance)
    {
        _randomNumberGenerator.seed(std::random_device()());

        // Take a deferral so the service isn't terminated
        _serviceDeferral = taskInstance->GetDeferral();

        taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &RandomNumberGeneratorTask::OnTaskCanceled);

        auto details = dynamic_cast<AppServiceTriggerDetails^>(taskInstance->TriggerDetails);
        _connection = details->AppServiceConnection;

        // Listen for incoming app service requests
        _connection->RequestReceived += ref new TypedEventHandler<AppServiceConnection^, AppServiceRequestReceivedEventArgs^>(OnRequestReceived);
    }

    void RandomNumberGeneratorTask::OnTaskCanceled(IBackgroundTaskInstance^ sender, BackgroundTaskCancellationReason reason)
    {
        if (_serviceDeferral != nullptr)
        {
            // Complete the service deferral
            _serviceDeferral->Complete();
            _serviceDeferral = nullptr;
        }

        delete _connection;
        _connection = nullptr;
    }

    void OnRequestReceived(AppServiceConnection^ sender, AppServiceRequestReceivedEventArgs^ args)
    {
        // Get a deferral so we can use an awaitable API to respond to the message
        auto messageDeferral = args->GetDeferral();

        auto input = args->Request->Message;
        int32 minValue = safe_cast<int32>(input->Lookup("minvalue"));
        int32 maxValue = safe_cast<int32>(input->Lookup("maxvalue"));
        std::uniform_int_distribution<std::mt19937::result_type> distribution(minValue, maxValue); // distribution in range [minValue, maxValue]
        int randomNum = distribution(_randomNumberGenerator);

        // Create the response
        auto result = ref new ValueSet();
        result->Insert("result", randomNum);

        // Send the response asynchronously
        create_task(args->Request->SendResponseAsync(result)).then([messageDeferral](AppServiceResponseStatus status)
        {
            // Complete the message deferral so the platform knows we're done responding
            messageDeferral->Complete();
        });
    }
}