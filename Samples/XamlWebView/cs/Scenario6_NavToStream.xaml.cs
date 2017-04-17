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
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario6_NavToStream : Page
    {
        private StreamUriWinRTResolver myResolver;

        public Scenario6_NavToStream()
        {
            myResolver = new StreamUriWinRTResolver();
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // The 'Host' part of the URI for the ms-local-stream protocol is a combination of the package name
            // and an application-defined key, which identifies the specfic resolver.
            // Here, we use 'MyTag'.

            Uri url = WebViewControl.BuildLocalStreamUri("MyTag", "/default.html");

            // The resolver object needs to be passed in to the navigate call.
            WebViewControl.NavigateToLocalStreamUri(url, myResolver);
        }

        /// <summary>
        /// Sample URI resolver object for use with NavigateToLocalStreamUri.
        /// The object must implement the IUriToStreamResolver interface
        /// 
        /// Note: If your content is stored in the package or local state,
        /// then you can use the ms-appx-web: or ms-appdata: protocol demonstrated in the
        /// "Navigate to package and local state" scenario. Those protocols will
        /// will create a resolver for you.
        /// </summary>
        public sealed class StreamUriWinRTResolver : Windows.Web.IUriToStreamResolver
        {
            /// <summary>
            /// The entry point for resolving a URI to a stream.
            /// </summary>
            /// <param name="uri"></param>
            /// <returns></returns>
            public IAsyncOperation<IInputStream> UriToStreamAsync(Uri uri)
            {
                // Because of the signature of the this method, it can't use await, so we 
                // call into a separate helper method that can use the C# await pattern.
                return GetContentAsync(uri).AsAsyncOperation();
            }


            /// <summary>
            /// Helper that produces the contents corresponding to a Uri.
            /// Uses the C# await pattern to coordinate async operations.
            /// </summary>
            /// <param name="uri"></param>
            /// <returns></returns>
            private async Task<IInputStream> GetContentAsync(Uri uri)
            {
                string path = uri.AbsolutePath;
                string contents;

                switch (path)
                {
                    case "/default.html":
                        contents = await MainPage.LoadStringFromPackageFileAsync("stream_example.html");
                        contents = contents.Replace("%", Windows.ApplicationModel.Package.Current.Id.Name);
                        break;

                    case "/stream.css":
                        contents = "p { color: blue; }";
                        break;

                    default:
                        throw new Exception($"Could not resolve URI \"{uri}\"");
                }

                // Convert the string to a stream.
                IBuffer buffer = CryptographicBuffer.ConvertStringToBinary(contents, BinaryStringEncoding.Utf8);
                var stream = new InMemoryRandomAccessStream();
                await stream.WriteAsync(buffer);
                return stream.GetInputStreamAt(0);
            }
        }
    }
}
