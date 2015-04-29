using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDKTemplate
{
    class Helpers
    {
        public static void InitializeRevisionNumber()
        {
            // This initializes the value used throughout the sample which tracks the expected index revision number. This
            // value is used to check if the app's expected revision number matches the actual index revision number and is
            // stored in the app's local settings to allow it to persist across termination.
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            if (!localSettings.Values.ContainsKey("expectedIndexRevision"))
            {
                localSettings.Values.Add("expectedIndexRevision", (ulong)0);
            }
        }

        public static void OnIndexerOperationBegin()
        {
            // Update the expected index revision number in expectation of the call that's about to be made
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            var revisionNumber = localSettings.Values["expectedIndexRevision"];
            localSettings.Values["expectedIndexRevision"] = (ulong)((ulong)revisionNumber + 1);
        }

        public static void OnIndexerOperationComplete(Windows.Storage.Search.ContentIndexer indexer)
        {
            // Set the expectedIndexRevision to be the reported index revision number because the operation
            // has succeeded
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            localSettings.Values["expectedIndexRevision"] = indexer.Revision; 
        }

        /// <summary>
        /// This function takes a string or vector string property value and returns a flattened string representation of it.
        /// </summary>
        public static string StringifyProperty(object property)
        {
            string propertyString = "";
            if (property != null)
            {
                var vectorProperty = property as IEnumerable<string>;
                if (vectorProperty != null)
                {
                    foreach (var prop in vectorProperty)
                    {
                        propertyString += prop + "; ";
                    }
                }
                else
                {
                    propertyString = property.ToString();
                }
            }
            return propertyString;
        }

        public static string CreateItemString(string itemKey, IEnumerable<string> propertyKeys, IReadOnlyDictionary<string, object> properties)
        {
            string itemString = "Key: " + itemKey + "\n";
            foreach (var propertyKey in propertyKeys)
            {
                itemString += propertyKey + ": " + StringifyProperty(properties[propertyKey]) + "\n";
            }
            return itemString;
        }

        public static string CreateItemString(string itemKey, IEnumerable<string> propertyKeys, IDictionary<string, object> properties)
        {
            string itemString = "Key: " + itemKey + "\n";
            foreach (var propertyKey in propertyKeys)
            {
                itemString += propertyKey + ": " + StringifyProperty(properties[propertyKey]) + "\n";
            }
            return itemString;
        }

        /// <summary>
        /// This function inserts three items into the index with generic Key, Name, Keywords, and Comment properties.
        /// </summary>
        public async static Task<string> AddItemsToIndex(Windows.Storage.Search.ContentIndexer indexer)
        {
            var content = new Windows.Storage.Search.IndexableContent();
            const int numberOfItemsToIndex = 3;
            for (int i = 0; i < numberOfItemsToIndex; i++)
            {
                content.Properties[Windows.Storage.SystemProperties.ItemNameDisplay] = "Sample Item Name " + i;
                content.Properties[Windows.Storage.SystemProperties.Keywords] = "Sample keyword " + i;
                content.Properties[Windows.Storage.SystemProperties.Comment] = "Sample comment " + i;
                content.Id = "SampleKey" + i;
                OnIndexerOperationBegin();
                await indexer.AddAsync(content);
                OnIndexerOperationComplete(indexer);
            }
            string[] propertiesToQuery =
            {
                Windows.Storage.SystemProperties.ItemNameDisplay,
                Windows.Storage.SystemProperties.Keywords,
                Windows.Storage.SystemProperties.Comment
            };
            var query = indexer.CreateQuery("*", propertiesToQuery);
            uint count = await query.GetCountAsync();
            return "Number of items currently in the index: " + count;
        }

        /// <summary>
        /// For the purposes of this sample, the appcontent-ms files are stored in an "appcontent-ms" folder in the
        /// install directory. These are then copied into the app's "LocalState\Indexed" folder, which exposes them
        /// to the indexer.
        /// </summary>
        public async static Task<string> AddAppContentFilesToIndexedFolder()
        {
            var localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            var installDirectory = Windows.ApplicationModel.Package.Current.InstalledLocation;
            var outputString = "Items added to the \"Indexed\" folder:";
            var appContentFolder = await installDirectory.GetFolderAsync("appcontent-ms");
            var indexedFolder = await localFolder.CreateFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.OpenIfExists);
            var files = await appContentFolder.GetFilesAsync();
            foreach (var file in files)
            {
                outputString += "\n" + file.DisplayName + file.FileType;
                await file.CopyAsync(indexedFolder, file.Name, Windows.Storage.NameCollisionOption.ReplaceExisting);
            }
            return outputString;
        }
    }
}
