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
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage.Streams;

namespace SDKSample.HttpClientSample
{
    class SlowInputStream : IInputStream
    {
        uint length;
        uint position;

        public SlowInputStream(uint length)
        {
            this.length = length;
            position = 0;
        }

        public IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
        {
            return AsyncInfo.Run<IBuffer, uint>(async (cancellationToken, progress) =>
            {
                if (length - position < count)
                {
                    count = length - position;
                }

                byte[] data = new byte[count];
                for (uint i = 0; i < count; i++)
                {
                    data[i] = 64;
                }

                // Introduce a 1 second delay.
                await Task.Delay(1000);

                position += count;
                progress.Report(count);

                return data.AsBuffer();
            });
        }

        public void Dispose()
        {
        }
    }
}
