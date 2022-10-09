#include "pch.h"
#include "RandomNumberGeneratorTask.h"
#include "RandomNumberGeneratorTask.g.cpp"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::AppService;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::RandomNumberService::implementation
{
    void RandomNumberGeneratorTask::Run(IBackgroundTaskInstance const& taskInstance)
    {
        //Take a service deferral so the service isn't terminated
        serviceDeferral = taskInstance.GetDeferral();

        taskInstance.Canceled({ get_weak(), &RandomNumberGeneratorTask::OnTaskCanceled });

        //Initialize the random number generator
        randomNumberGenerator.seed(std::random_device()());

        auto details = taskInstance.TriggerDetails().as<AppServiceTriggerDetails>();
        connection = details.AppServiceConnection();

        //Listen for incoming app service requests
        connection.RequestReceived({ get_weak(), &RandomNumberGeneratorTask::OnRequestReceived });
    }

    void RandomNumberGeneratorTask::OnTaskCanceled(IBackgroundTaskInstance const&, BackgroundTaskCancellationReason const&)
    {
        if (connection != nullptr)
        {
            connection.Close();
            connection = nullptr;
        }

        if (serviceDeferral != nullptr)
        {
            //Complete the service deferral
            serviceDeferral.Complete();
            serviceDeferral = nullptr;
        }
    }

    fire_and_forget RandomNumberGeneratorTask::OnRequestReceived(AppServiceConnection const&, AppServiceRequestReceivedEventArgs const& args)
    {
        //Get a deferral so we can use an awaitable API to respond to the message
        auto messageDeferral = args.GetDeferral();

        ValueSet input = args.Request().Message();
        int minValue = input.TryLookup(L"minvalue").try_as<int>().value_or(0);
        int maxValue = input.TryLookup(L"maxvalue").try_as<int>().value_or(0);
        if (maxValue < minValue)
        {
            maxValue = minValue;
        }

        std::uniform_int_distribution<std::mt19937::result_type> distribution(minValue, maxValue); // distribution in range [minValue, maxValue]   
        int randomValue = distribution(randomNumberGenerator);

        //Create the response
        ValueSet result;
        result.Insert(L"result", box_value(randomValue));

        //Send the response
        co_await args.Request().SendResponseAsync(result);

        //Destruction of the message deferral lets the platform know we're done responding
    }

}
