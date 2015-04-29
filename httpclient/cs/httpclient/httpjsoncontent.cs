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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.Web.Http;
using Windows.Web.Http.Headers;

namespace SDKSample.HttpClientSample
{
    class HttpJsonContent : IHttpContent
    {
        IJsonValue jsonValue;
        HttpContentHeaderCollection headers;

        public HttpContentHeaderCollection Headers
        {
            get
            {
                return headers;
            }
        }

        public HttpJsonContent(IJsonValue jsonValue)
        {
            if (jsonValue == null)
            {
                throw new ArgumentException("jsonValue cannot be null.");
            }
            this.jsonValue = jsonValue;
            headers = new HttpContentHeaderCollection();
            headers.ContentType = new HttpMediaTypeHeaderValue("application/json");
            headers.ContentType.CharSet = "UTF-8";
        }

        public IAsyncOperationWithProgress<ulong, ulong> BufferAllAsync()
        {
            return AsyncInfo.Run<ulong, ulong>((cancellationToken, progress) =>
            {
                return Task<ulong>.Run(() =>
                {
                    ulong length = GetLength();

                    // Report progress.
                    progress.Report(length);

                    // Just return the size in bytes.
                    return length;
                });
            });
        }

        public IAsyncOperationWithProgress<IBuffer, ulong> ReadAsBufferAsync()
        {
            return AsyncInfo.Run<IBuffer, ulong>((cancellationToken, progress) =>
            {
                return Task<IBuffer>.Run(() =>
                {
                    DataWriter writer = new DataWriter();
                    writer.WriteString(jsonValue.Stringify());

                    // Make sure that the DataWriter destructor does not free the buffer.
                    IBuffer buffer = writer.DetachBuffer();

                    // Report progress.
                    progress.Report(buffer.Length);

                    return buffer;
                });
            });
        }

        public IAsyncOperationWithProgress<IInputStream, ulong> ReadAsInputStreamAsync()
        {
            return AsyncInfo.Run<IInputStream, ulong>(async (cancellationToken, progress) =>
            {
                InMemoryRandomAccessStream randomAccessStream = new InMemoryRandomAccessStream();
                DataWriter writer = new DataWriter(randomAccessStream);
                writer.WriteString(jsonValue.Stringify());

                uint bytesStored = await writer.StoreAsync().AsTask(cancellationToken);

                // Make sure that the DataWriter destructor does not close the stream.
                writer.DetachStream();

                // Report progress.
                progress.Report(randomAccessStream.Size);

                return randomAccessStream.GetInputStreamAt(0);
            });
        }

        public IAsyncOperationWithProgress<string, ulong> ReadAsStringAsync()
        {
            return AsyncInfo.Run<string, ulong>((cancellationToken, progress) =>
            {
                return Task<string>.Run(() =>
                {
                    string jsonString = jsonValue.Stringify();

                    // Report progress (length of string).
                    progress.Report((ulong)jsonString.Length);

                    return jsonString;
                });
            });
        }

        public bool TryComputeLength(out ulong length)
        {
            length = GetLength();
            return true;
        }

        public IAsyncOperationWithProgress<ulong, ulong> WriteToStreamAsync(IOutputStream outputStream)
        {
            return AsyncInfo.Run<ulong, ulong>(async (cancellationToken, progress) =>
            {
                DataWriter writer = new DataWriter(outputStream);
                writer.WriteString(jsonValue.Stringify());
                uint bytesWritten = await writer.StoreAsync().AsTask(cancellationToken);

                // Make sure that DataWriter destructor does not close the stream.
                writer.DetachStream();

                // Report progress.
                progress.Report(bytesWritten);

                return bytesWritten;
            });
        }

        public void Dispose()
        {
        }

        private ulong GetLength()
        {
            DataWriter writer = new DataWriter();
            writer.WriteString(jsonValue.Stringify());

            IBuffer buffer = writer.DetachBuffer();
            return buffer.Length;
        }
    }
}
