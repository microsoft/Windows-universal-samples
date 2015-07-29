#pragma once

// Reads Stream into Destination. It's up to caller to ensure that Destination is not destructed until
// ReadStreamTask::GetTask() is completed. If error is encountered during read Destination doesn't
// change and Stream state is undefined.
// NOTE: Generally you don't need to read entire stream into memory - we only use this task in order to
//       interoperate with Compression API style buffer compression/decompression. Recommended way to
//       work with WinRT streams of unknown length (including Comressor/Decompressor) is processing
//       data chunk by chunk as it arrives.
class ReadStreamTask
{
public:
    ReadStreamTask(Windows::Storage::Streams::IInputStream^ Stream, std::vector<byte> &Destination);    
    ReadStreamTask(const ReadStreamTask& Task);
    ReadStreamTask &operator=(const ReadStreamTask& Task);

    concurrency::task<size_t> RunTask();

private:
    static const unsigned int MinReadChunkSize = 0x10000;

    struct ReadStreamTaskImpl;
    ReadStreamTaskImpl *_Impl;
};

// Common context for all scenarios for simplicity - not all fields are used by every scenario
struct ScenarioContext
{
    Windows::Storage::Streams::InMemoryRandomAccessStream^  raStream;
    Windows::Storage::Compression::Compressor^              compressor;
    Windows::Storage::Streams::DataWriter^                  writer;
    std::vector<byte>                                       originalData;
    std::vector<byte>                                       compressedData;
    std::vector<byte>                                       decompressedData;
};
