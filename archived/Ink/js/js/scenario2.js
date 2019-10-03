//// Copyright (c) Microsoft Corporation. All rights reserved

// Sample app demonstrating the use of InkPresenter APIs.

(function () {
    "use strict";

    // abbreviations
    var Inking = Windows.UI.Input.Inking;
    var InkInputProcessingMode = Inking.InkInputProcessingMode;

    function displayStatus(message) {
        WinJS.log && WinJS.log(message, "sample", "status");
    }

    function displayError(message) {
        WinJS.log && WinJS.log(message, "sample", "error");
    }

    var STROKES_FILENAME = "strokes.txt";

    var inkCanvas;
    var inkContext;
    var inkPresenter;
    var inkColor;

    // Undo the last stroke by removing it from the stroke container.
    function undo() {
        var strokeContainer = inkPresenter.strokeContainer;
        var strokes = strokeContainer.getStrokes();

        // If there are no strokes, there's nothing to undo.
        if (strokes.length === 0) {
            return;
        }

        // Select the last stroke and delete it.
        strokes[strokes.length - 1].selected = true;
        strokeContainer.deleteSelected();
    }

    // Switch pen tip to eraser mode.
    function eraseMode() {
        inkPresenter.inputProcessingConfiguration.mode = InkInputProcessingMode.erasing;
    }

    // Switch pen tip to pencil mode.
    function pencilMode() {
        var drawingAttributes = new Inking.InkDrawingAttributes.createForPencil();
        drawingAttributes.ignorePressure = false;
        drawingAttributes.fitToCurve = true;
        drawingAttributes.color = inkColor;
        drawingAttributes.size = { width: 3, height: 3 };
        inkPresenter.updateDefaultDrawingAttributes(drawingAttributes);
        inkPresenter.inputProcessingConfiguration.mode = InkInputProcessingMode.inking
    }

    // Switch pen tip to highlighter mode.
    function highlighterMode() {
        var drawingAttributes = new Inking.InkDrawingAttributes();
        drawingAttributes.ignorePressure = true;
        drawingAttributes.fitToCurve = true;
        drawingAttributes.drawAsHighlighter = true;
        drawingAttributes.penTip = Inking.PenTipShape.rectangle;
        drawingAttributes.size = { width: 10, height: 10 };
        drawingAttributes.color = inkColor;
        inkPresenter.updateDefaultDrawingAttributes(drawingAttributes);
        inkPresenter.inputProcessingConfiguration.mode = InkInputProcessingMode.inking;
    }

    // Load the strokes from the file into the stroke container.
    function load() {
        var strokeContainer = inkPresenter.strokeContainer;
        var loadStream;

        var localAppDataFolder = Windows.Storage.ApplicationData.current.localFolder;
        localAppDataFolder.getFileAsync(STROKES_FILENAME).then(function (file) {
            // Open the file
            return file.openAsync(Windows.Storage.FileAccessMode.read);
        }).then(function (stream) {
            // Load the strokes into the stroke container.
            loadStream = stream;
            return strokeContainer.loadAsync(stream)
        }).done(function () {
            // Input stream is IClosable interface and requires explicit close.
            loadStream.close();
        },
        function () {
            displayError("Load Failed. First save a file before loading one.");
        });
    }

    // Save the strokes from the stroke container to a file.
    function save() {
        var strokeContainer = inkPresenter.strokeContainer;
        var saveStream;

        // Create or replace existing file.
        var localAppDataFolder = Windows.Storage.ApplicationData.current.localFolder;
        localAppDataFolder.createFileAsync(STROKES_FILENAME, Windows.Storage.CreationCollisionOption.replaceExisting).then(function (file) {
            // Open the file
            return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
        }).then(function (stream) {
            // Save the strokes to the file.
            saveStream = stream;
            return strokeContainer.saveAsync(stream);
        }).then(function () {
            return saveStream.flushAsync();
        }).done(function () {
            // Output stream is IClosable interface and requires explicit close.
            saveStream.close();
        });
    }

    // Clear the stroke container and display text.
    function clear() {
        inkPresenter.strokeContainer.clear();

        displayStatus("");
        displayError("");
    }

    // A button handler which fetches the ID from the button, which should be a color name.
    // Set the ink presenter drawing attributes' color to that color.
    function setInkColor(evt) {
        switch (evt.currentTarget.id) {
            case "Black":
                inkColor = Windows.UI.Colors.black;
                break;
            case "Blue":
                inkColor = Windows.UI.Colors.blue;
                break;
            case "Red":
                inkColor = Windows.UI.Colors.red;
                break;
            case "Green":
                inkColor = Windows.UI.Colors.green;
                break;
        }

        var drawingAttributes = inkPresenter.copyDefaultDrawingAttributes();
        drawingAttributes.color = inkColor;
        inkPresenter.updateDefaultDrawingAttributes(drawingAttributes);
    }

    function inkInitialize() {
        // Utility to fetch elements by ID.
        function id(elementId) {
            return document.getElementById(elementId);
        }

        WinJS.UI.processAll().then(
            function () {
                id("Black").addEventListener("click", setInkColor);
                id("Blue").addEventListener("click", setInkColor);
                id("Red").addEventListener("click", setInkColor);
                id("Green").addEventListener("click", setInkColor);

                inkCanvas = id("InkCanvas");
                inkCanvas.setAttribute("width", inkCanvas.offsetWidth);
                inkCanvas.setAttribute("height", inkCanvas.offsetHeight);

                // Get the ms-ink context
                inkContext = inkCanvas.getContext("ms-ink");
                inkPresenter = inkContext.msInkPresenter;

                inkPresenter.inputDeviceTypes =
                    Windows.UI.Core.CoreInputDeviceTypes.mouse |
                    Windows.UI.Core.CoreInputDeviceTypes.pen;

                // Set the default drawing attributes.
                inkColor = Windows.UI.Colors.black;
                pencilMode();
            }
        ).done(null, function (e) {
            displayError("inkInitialize " + e.toString());
        });
    }

    // Tag the event handlers of the ToolBar so that they can be used in a declarative context.
    // For security reasons WinJS.UI.processAll and WinJS.Binding.processAll (and related) functions allow only
    // functions that are marked as being usable declaratively to be invoked through declarative processing.
    WinJS.UI.eventHandler(undo);
    WinJS.UI.eventHandler(eraseMode);
    WinJS.UI.eventHandler(pencilMode);
    WinJS.UI.eventHandler(highlighterMode);
    WinJS.UI.eventHandler(load);
    WinJS.UI.eventHandler(save);
    WinJS.UI.eventHandler(clear);
    WinJS.Namespace.define("Ink", {
        undo: undo,
        eraseMode: eraseMode,
        pencilMode: pencilMode,
        highlighterMode: highlighterMode,
        load: load,
        save: save,
        clear: clear,
    });

    // As a workaround, we must temporarily change the size of the canvas in order to force a redraw.
    // Otherwise, the ink strokes will not be visible on resume.
    document.addEventListener("visibilitychange", function () {
        if (document.visibilityState === "visible") {
            requestAnimationFrame(function () {
                inkCanvas = document.getElementById("InkCanvas");
                inkCanvas.width = inkCanvas.width + 1;
                inkCanvas.width = inkCanvas.width - 1;
            });
        }
    });

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            inkInitialize();
        }
    });
})();