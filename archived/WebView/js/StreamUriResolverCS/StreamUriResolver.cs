using System;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Foundation;
using Windows.Web;
using Windows.Security.Cryptography;

namespace SDK.WebViewSampleCS
{
    public sealed class StreamUriResolver : IUriToStreamResolver
    {
        public IAsyncOperation<IInputStream> UriToStreamAsync(System.Uri uri)
        {
            // The WebView's buildLocalStreamUri method takes contentIdentifier and relativePath
            // parameters to generate a URI of the form:
            //     ms-local-stream://<package name>_<encoded contentIdentifier>/<relativePath>
            // The resolver can decode the contentIdentifier to determine what content should be
            // returned in the output stream.
            string host = uri.Host;
            int delimiter = host.LastIndexOf('_');
            string encodedContentId = host.Substring(delimiter + 1);
            IBuffer buffer = CryptographicBuffer.DecodeFromHexString(encodedContentId);

            string contentIdentifier = CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, buffer);
            string relativePath = uri.PathAndQuery;

            // For this sample, we will return a stream for a file under the local app data
            // folder, under the subfolder named after the contentIdentifier and having the
            // given relativePath.  Real apps can have more complex behavior, such as handling
            // contentIdentifiers in a custom manner (not necessarily as paths), and generating
            // arbitrary streams that are not read directly from a file.
            System.Uri appDataUri = new Uri("ms-appdata:///local/" + contentIdentifier + relativePath);

            return GetFileStreamFromApplicationUriAsync(appDataUri).AsAsyncOperation();
        }

        private async Task<IInputStream> GetFileStreamFromApplicationUriAsync(System.Uri uri)
        {
            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(uri);
            IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read);
            return stream;
        }
    }
}
