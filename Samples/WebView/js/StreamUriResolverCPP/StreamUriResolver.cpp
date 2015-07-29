// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "StreamUriResolver.h"
#include <ppltasks.h>
#include <strsafe.h>
#include <assert.h>


using namespace SDK::WebViewSampleCpp;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace concurrency;

namespace SDK
{
    namespace WebViewSampleCpp
    {

        IAsyncOperation<IInputStream^>^ StreamUriResolver::UriToStreamAsync(Uri^ uri)
        {
            // The WebView's buildLocalStreamUri method takes contentIdentifier and relativePath
            // parameters to generate a URI of the form:
            //     ms-local-stream://<package name>_<encoded contentIdentifier>/<relativePath>
            // The resolver can decode the contentIdentifier to determine what content should be
            // returned in the output stream.
            String^ uriHost = uri->Host;
            PCWSTR encodedContentIdentifier = wcschr(uriHost->Data(), L'_');
            if (encodedContentIdentifier == nullptr)
            {
                throw ref new InvalidArgumentException();
            }
            else
            {
                encodedContentIdentifier++;
            }

            IBuffer^ buffer = CryptographicBuffer::DecodeFromHexString(ref new String(encodedContentIdentifier));
            String^ contentIdentifier = CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, buffer);
            String^ relativePath = ref new String(uri->Path->Data());

            // For this sample, we will return a stream for a file under the local app data
            // folder, under the subfolder named after the contentIdentifier and having the
            // given relativePath.  Real apps can have more complex behavior, such as handling
            // contentIdentifiers in a custom manner (not necessarily as paths), and generating
            // arbitrary streams that are not read directly from a file.
            Uri^ appDataUri = ref new Uri(L"ms-appdata:///local/" + contentIdentifier + relativePath);

            StreamRetrievalOperation^ asyncOp = ref new StreamRetrievalOperation();
            asyncOp->Start(appDataUri);
            return asyncOp;
        }

        StreamRetrievalOperation::StreamRetrievalOperation()
        {
            _result = nullptr;
            _completedHandler = nullptr;
            _asyncStatus = AsyncStatus::Started;
            _errorCode.Value = S_OK;
            _firedCompletion = false;
            _isClosed = false;
        }

        void StreamRetrievalOperation::Start(Uri^ uri)
        {
            create_task(StorageFile::GetFileFromApplicationUriAsync(uri), task_continuation_context::use_arbitrary()).then([this](StorageFile^ storageFile)
            {
                return create_task(storageFile->OpenAsync(FileAccessMode::Read), task_continuation_context::use_arbitrary());
            }, task_continuation_context::use_arbitrary()).then([this](task<IRandomAccessStream^> task)
            {
                // The final lamdba takes the task instead of the IRandomAccessStream as the parameter so
                // get can be called explicitly on the task. Get will throw an exception if the task
                // failed. The completed handler needs to be invoked whether the task succeeds or fails.
                try
                {
                    IRandomAccessStream^ stream = task.get();
                    // The IRandomAccessStream can be cast to an IInputStream because IRandomAccessStream
                    // requires IInputStream.
                    IInputStream^ inputStream = static_cast<IInputStream^>(stream);
                    TransitionToCompleted(inputStream);
                }
                catch (Platform::COMException^ e)
                {
                    TransitionToError(e);
                }
            }, task_continuation_context::use_arbitrary());
        }

        void StreamRetrievalOperation::Cancel()
        {
            _asyncStatus = AsyncStatus::Canceled;
            FireCompletion();
        }

        void StreamRetrievalOperation::Close()
        {
            _isClosed = true;
            _result = nullptr;
            _completedHandler = nullptr;
        }

        IInputStream^ StreamRetrievalOperation::GetResults()
        {
            if (_isClosed)
            {
                throw ref new Exception(E_ILLEGAL_METHOD_CALL);
            }

            return _result;
        }

        void StreamRetrievalOperation::Completed::set(AsyncOperationCompletedHandler<IInputStream^>^ value)
        {
            _completedHandler = value;
            FireCompletion();
        }

        AsyncOperationCompletedHandler<IInputStream^>^ StreamRetrievalOperation::Completed::get()
        {
            if (_isClosed)
            {
                throw ref new Exception(E_ILLEGAL_METHOD_CALL);
            }

            return _completedHandler;
        }

        void StreamRetrievalOperation::TransitionToCompleted(IInputStream^ inputStream)
        {
            if (_asyncStatus == AsyncStatus::Started)
            {
                _result = inputStream;
                _asyncStatus = AsyncStatus::Completed;
                FireCompletion();
            }
        }

        void StreamRetrievalOperation::TransitionToError(COMException^ exception)
        {
            if (_asyncStatus == AsyncStatus::Started)
            {
                _result = nullptr;
                _asyncStatus = AsyncStatus::Error;
                _errorCode.Value = exception->HResult;
                FireCompletion();
            }
        }

        void StreamRetrievalOperation::FireCompletion()
        {
            // Fire the completed handler once, and only after
            // a completed handler has been set and the operation
            // has finished.
            if (_asyncStatus != AsyncStatus::Started &&
                _completedHandler != nullptr &&
                !_firedCompletion)
            {
                _firedCompletion = true;
                _completedHandler(this, _asyncStatus);
            }
        }

    } // namespace WebViewSampleCpp
} // namespace SDK