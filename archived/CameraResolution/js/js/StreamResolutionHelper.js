//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

function streamResolutionHelper(encodingProperties) {
    this.encodingProperties = encodingProperties;
}

/// <summary>
/// Output properties to a readable format for UI purposes
/// eg. 1920x1080 [1.78] 30fps MPEG
/// </summary>
/// <param name="showFrameRate"></param>
/// <returns>Readable string</returns>
streamResolutionHelper.prototype.getFriendlyName = function (showFrameRate) {
    if (this.encodingProperties.type === "Image" || !showFrameRate) {
        return this.width() + "x" + this.height() + " [" + this.aspectRatio() + "] " +
            this.encodingProperties.subtype;
    }
    else if (this.encodingProperties.type == "Video") {
        return this.width() + "x" + this.height() + " [" + this.aspectRatio() + "] " +
            this.frameRate() + "FPS " + this.encodingProperties.subtype;
    }
}

streamResolutionHelper.prototype.width = function () {
    if (this.encodingProperties.type === "Image" || 
        this.encodingProperties.type === "Video") {
        return this.encodingProperties.width;
    }
    
    return 0;
}

streamResolutionHelper.prototype.height = function () {
    if (this.encodingProperties.type === "Image" ||
        this.encodingProperties.type === "Video") {
        return this.encodingProperties.height;
    }

    return 0;
}

streamResolutionHelper.prototype.frameRate = function () {
    if (this.encodingProperties.type === "Video") {
        if (this.encodingProperties.frameRate.denominator != 0) {
            return Math.round(this.encodingProperties.frameRate.numerator / this.encodingProperties.frameRate.denominator);
        }
    }

    return 0;
}

streamResolutionHelper.prototype.aspectRatio = function () {
    return (this.width() / this.height()).toFixed(2);
}

/// <summary>
/// Sets encoding properties on a camera stream. Ensures VideoElement and preview stream are stopped before setting properties.
/// </summary>
function setMediaStreamPropertiesAsync(mediaCapture, streamType, encodingProperties) {
    // Stop preview and unlink the VideoElement from the MediaCapture object
    var previewVidTag = document.getElementById("cameraPreview");
    previewVidTag.pause;
    previewVidTag.src = null;

    // Apply desired stream properties
    return mediaCapture.videoDeviceController.setMediaStreamPropertiesAsync(streamType, encodingProperties)
        .then(function () {
            // Recreate pipeline and restart the preview
            var previewVidTag = document.getElementById("cameraPreview");
            var previewUrl = URL.createObjectURL(mediaCapture);
            previewVidTag.src = previewUrl;
            previewVidTag.play();
        });
}