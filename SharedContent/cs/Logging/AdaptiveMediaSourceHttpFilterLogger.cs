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

using SDKTemplate.Logging.Extensions;
using System;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace SDKTemplate.Logging
{
    public class AdaptiveMediaSourceHttpFilterLogger : IHttpFilter
    {
        #region Common pattern for all loggers

        private LogView logView;
        private IHttpFilter innerFilter;
        private Stopwatch timeToResponseComplete;

        public AdaptiveMediaSourceHttpFilterLogger(LogView _logView, IHttpFilter _innerFilter)
        {
            if (_logView == null)
                throw new ArgumentNullException(nameof(_logView));
            this.logView = _logView;

            if (_innerFilter == null)
                throw new ArgumentNullException(nameof(_innerFilter));
            this.innerFilter = _innerFilter;

            this.timeToResponseComplete = new Stopwatch();
        }

        public void Dispose()
        {

            this.timeToResponseComplete = null;
            this.logView = null;
            this.innerFilter = null;
        }
        
        #endregion

        public IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> SendRequestAsync(HttpRequestMessage request)
        {
            timeToResponseComplete.Start();
            logView.Log($"{{\"FILTER SendRequestAsync\":{{\"requestUri\":\"{request.RequestUri}\"}}}}", LogViewLoggingLevel.Verbose);

            return AsyncInfo.Run<HttpResponseMessage, HttpProgress>(async (cancellationToken, operationProgressReporter) =>
            {
                IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> innerOperation = innerFilter.SendRequestAsync(request);

                innerOperation.Progress += new AsyncOperationProgressHandler<HttpResponseMessage, HttpProgress>(
                    (IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> asyncInfo, HttpProgress httpProgressInfo) =>
                {
                    logView.Log($"{{\"FILTER Progress Handler\": {{\"requestUri\":\"{request.RequestUri}\", {httpProgressInfo.ToJsonObject()}}}}}", LogViewLoggingLevel.Verbose);
                });

                HttpResponseMessage response = await innerOperation.AsTask(cancellationToken, operationProgressReporter);
                cancellationToken.ThrowIfCancellationRequested();
                // At this point, SendRequest has completed and we have a response.
                timeToResponseComplete.Stop();

                // We could inspect the content of the response headers:
                var countHeaders = response.Headers.Count();
                logView.Log($"{{\"FILTER Response Complete\": {{\"StatusCode\":\"{response.StatusCode}\",\"timeToResponseComplete\":\"{timeToResponseComplete.ElapsedMilliseconds}\",\"requestUri\":\"{request.RequestUri}\",\"countHeaders\":\"{countHeaders}\"}}}}", LogViewLoggingLevel.Verbose);

                if(!response.IsSuccessStatusCode)
                {
                    logView.Log($"{{\"FILTER Response Failed\": {{\"StatusCode\":\"{response.StatusCode}\",\"timeToResponseComplete\":\"{timeToResponseComplete.ElapsedMilliseconds}\",\"requestUri\":\"{request.RequestUri}\",\"countHeaders\":\"{countHeaders}\"}}}}", LogViewLoggingLevel.Error);
                }

                // Note that we should NOT attempt to access the response.Content in an IHttpFilter such as this,
                // the Content stream is for the base filter and AdaptiveMediaSource to use.
                //
                // If you want to download content in the App and then pass it to the AdaptiveMediaSource, 
                // see AppDownloadedKeyRequest in Scenario3, which uses Deferral objects to manage thread timing
                // with the AdaptiveMediaSource.
                return response;
            });
        }

    }
}
