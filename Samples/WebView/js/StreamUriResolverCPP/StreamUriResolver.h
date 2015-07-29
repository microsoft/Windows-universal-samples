// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

namespace SDK
{
    namespace WebViewSampleCpp
    {
        public ref class StreamUriResolver sealed : public Windows::Web::IUriToStreamResolver
        {
        public:
            virtual Windows::Foundation::IAsyncOperation<Windows::Storage::Streams::IInputStream^>^ UriToStreamAsync(Windows::Foundation::Uri^ uri);

        };

        // IAsyncOperation implementation to be returned from UriToStreamAsync. This class is used instead of create_async
        // because Parallel Patterns Library (PPL) will marshal any calls back to the original thread. The async operation for UriToStreamAsync can
        // complete and call the completed handler on a different thread than the operation was started on. StreamResolvers that are loading many
        // resources at the same time can manifest in multiple completions occuring at the same time on the original thread and therefore preventing
        // optimal parallelization of the work.
        public ref class StreamRetrievalOperation sealed : public Windows::Foundation::IAsyncOperation<Windows::Storage::Streams::IInputStream^>
        {
        internal:
            StreamRetrievalOperation();

            void Start(Windows::Foundation::Uri^ uri);

        public:
            virtual Windows::Storage::Streams::IInputStream^ GetResults();
            virtual void Close();
            virtual void Cancel();

            property Windows::Foundation::AsyncOperationCompletedHandler<Windows::Storage::Streams::IInputStream^>^ Completed {
                virtual Windows::Foundation::AsyncOperationCompletedHandler<Windows::Storage::Streams::IInputStream^>^ get();
                virtual void set(Windows::Foundation::AsyncOperationCompletedHandler<Windows::Storage::Streams::IInputStream^>^ value);
            }

            property unsigned int Id {
                virtual unsigned int get() { return 1; }
            }

            property Windows::Foundation::AsyncStatus Status {
                virtual Windows::Foundation::AsyncStatus get() { return _asyncStatus; }
            }

            property Windows::Foundation::HResult ErrorCode {
                virtual Windows::Foundation::HResult get() { return _errorCode; }
            }

        private:
            void TransitionToCompleted(Windows::Storage::Streams::IInputStream^ inputStream);
            void TransitionToError(Platform::COMException^ exception);
            void FireCompletion();

            Windows::Storage::Streams::IInputStream^ _result;
            Windows::Foundation::AsyncOperationCompletedHandler<Windows::Storage::Streams::IInputStream^>^ _completedHandler;
            Windows::Foundation::AsyncStatus _asyncStatus;
            Windows::Foundation::HResult _errorCode;
            bool _firedCompletion;
            bool _isClosed;

        };

    } // namespace WebViewSampleCpp
} // namespace SDK

