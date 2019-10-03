//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("getFiles").addEventListener("click", getFiles, false);
        }
    });


    // Get the Pictures library and enumerate all its files using the prefetch APIs
    function getFiles() {
        clearOutput();
        var copyrightProperty = "System.Copyright";
        var colorSpaceProperty = "System.Image.ColorSpace";
        var additionalProperties = [copyrightProperty, colorSpaceProperty];
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            var search = Windows.Storage.Search;
            var fileProperties = Windows.Storage.FileProperties;

            // Create query options with common query sort order and file type filter.
            var fileTypeFilter = [".jpg", ".png", ".bmp", ".gif"];
            var queryOptions = new search.QueryOptions(search.CommonFileQuery.orderByName, fileTypeFilter);

            // Set up property prefetch - use the PropertyPrefetchOptions for top-level properties
            // and an array for additional properties.
            var imageProperties = fileProperties.PropertyPrefetchOptions.imageProperties;

            queryOptions.setPropertyPrefetch(imageProperties, additionalProperties);

            // Set up thumbnail prefetch if needed, e.g. when creating a picture gallery view.
            /*
            var thumbnailMode = fileProperties.ThumbnailMode.picturesView;
            var requestedSize = 190;
            var thumbnailOptions = fileProperties.ThumbnailOptions.useCurrentScale;
            queryOptions.setThumbnailPrefetch(thumbnailMode, requestedSize, thumbnailOptions);
            */

            // Query the Pictures library.
            var query = picturesLibrary.createFileQueryWithOptions(queryOptions);
            return query.getFilesAsync();
        }).done(function (items) {
            // Output the query results
            items.forEach(function (item) {
                // Create an entry in the list for the item
                var list = document.getElementById("itemsList");
                var listItemElement = document.createElement("li");
                listItemElement.textContent = item.name;
                list.appendChild(listItemElement);

                // Create UI elements for the output and fill in the contents when they are available.
                var propertyList = document.createElement("ul");
                var dimensions = document.createElement("li");
                propertyList.appendChild(dimensions);
                var copyright = document.createElement("li");
                propertyList.appendChild(copyright);
                var colorSpace = document.createElement("li");
                propertyList.appendChild(colorSpace);
                list.appendChild(propertyList);

                // getImagePropertiesAsync will return synchronously when prefetching
                // has been able to retrieve the properties in advance.
                item.properties.getImagePropertiesAsync().done(function (properties) {
                    dimensions.textContent = "Dimensions: " + properties.width + "x" + properties.height;
                });

                // Similarly, extra properties are retrieved asynchronously but may
                // return immediately when prefetching has fulfilled its task.
                item.properties.retrievePropertiesAsync(additionalProperties).done(function (properties) {
                    copyright.textContent = "Copyright: " + (properties[copyrightProperty] ? properties[copyrightProperty] : "none");
                    colorSpace.textContent = "Color space: " + (properties[colorSpaceProperty] ? properties[colorSpaceProperty] : "none");
                });

                // Thumbnails can also be retrieved and used
                /*
                item.getThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions).done(function (thumbnail) {
                    // Use the thumbnail
                });
                */
            });
        });
    }

    function clearOutput() {
        document.getElementById("itemsList").innerHTML = "";
    }
})();
