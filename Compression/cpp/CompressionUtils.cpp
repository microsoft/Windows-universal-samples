#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"
#include "CompressionUtils.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Compression;

using namespace concurrency;

#pragma region ReadStreamTask implementation
// We can't derive from task because we need to initialize task_completion_event before task itself.
struct ReadStreamTask::ReadStreamTaskImpl {
    ReadStreamTaskImpl(IInputStream^ Stream, std::vector<byte> &Destination) :
        _Destination(Destination),
        _Reader(ref new DataReader(Stream)),
        _CompletionEvent()
    {
    }

    task<size_t> RunTask()
    {
        ReadChunk();
        return create_task(_CompletionEvent).then([this]
        {
            // Schedule resource deallocation
            std::unique_ptr<ReadStreamTaskImpl> thisHolder(this);

            // Prevent user provided stream from being closed
            _Reader->DetachStream();

            // Move data out
            _Destination = std::move(_StreamData);

            return _Destination.size();
        });
    }

    void ReadChunk()
    {
        // LoadAsync itself could throw exception if IAsyncOperation couldn't be created with
        // given parameters in a current state
        try
        {
            // Read data into the data reader
            _StreamData.reserve(_StreamData.size() + MinReadChunkSize);
            create_task(_Reader->LoadAsync(static_cast<unsigned int>(_StreamData.capacity() - _StreamData.size())))

            // Then store it to the result vector
            .then([this](task<unsigned int> BytesRead)
            {
                try
                {
                    // exception is thrown here if LoadAsync operation has been completed with an error
                    auto bytesRead = BytesRead.get();
                    if (bytesRead)
                    {
                        // Store chunk of data in the result vector.
                        auto newData = ref new Platform::Array<byte>(bytesRead);
                        _Reader->ReadBytes(newData);

                        _StreamData.insert(_StreamData.end(), newData->begin(), newData->end());

                        // Then recurse to read next chunk.
                        ReadChunk();
                    }
                    else
                    {
                        // End of stream is reached - complete top level task with the data we've read so far

                        // make sure that _CompletionEvent outlives continuation which deletes this ReadStreamTaskImpl
                        auto completionEvent = _CompletionEvent;
                        completionEvent.set();
                    }
                }
                catch (...)
                {
                    // Schedule resource deallocation
                    std::unique_ptr<ReadStreamTaskImpl> thisHolder(this);

                    _CompletionEvent.set_exception(std::current_exception());
                }
            });
        }
        catch (...)
        {
            // Schedule resource deallocation
            std::unique_ptr<ReadStreamTaskImpl> thisHolder(this);

            _CompletionEvent.set_exception(std::current_exception());
        }
    }

    // All data members are accessed serially - no synchronization is done
    std::vector<byte> &_Destination;
    DataReader^ _Reader;
    std::vector<byte> _StreamData;

    task_completion_event<void> _CompletionEvent;
};

ReadStreamTask::ReadStreamTask(IInputStream^ Stream, std::vector<byte> &Destination) :
    _Impl(new ReadStreamTaskImpl(Stream, Destination))
{
}

ReadStreamTask::ReadStreamTask(const ReadStreamTask& Task) :
    _Impl(Task._Impl)
{
}

ReadStreamTask &ReadStreamTask::operator=(const ReadStreamTask& Task)
{
    _Impl = Task._Impl;
    return *this;
}

task<size_t> ReadStreamTask::RunTask()
{
    return _Impl->RunTask();
}
#pragma endregion
