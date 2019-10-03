//// Copyright (c) Microsoft Corporation. All rights reserved

// Sample app demonstrating the use of Ink and Reco APIs.
// We are using Windows.UI.Input.Inking.InkManager.

(function () {
    "use strict";
    function displayStatus(message) {
        WinJS.log && WinJS.log(message, "sample", "status");
    }

    function displayError(message) {
        WinJS.log && WinJS.log(message, "sample", "error");
    }

    // Functions to convert from and to the 32-bit int used to represent color in Windows.UI.Input.Inking.InkManager.

    // Convenience function used by color converters.
    // Assumes arg num is a number (0..255); we convert it into a 2-digit hex string.

    function byteHex(num) {
        var hex = num.toString(16);
        if (hex.length === 1) {
            hex = "0" + hex;
        }
        return hex;
    }

    // Convert from Windows.UI.Input.Inking's color code to html's color hex string.

    function toColorString(color) {
        return "#" + byteHex(color.r) + byteHex(color.g) + byteHex(color.b);
    }

    // Convert from the few color names used in this app to Windows.UI.Input.Inking's color code.
    // If it isn't one of those, then decode the hex string.  Otherwise return gray.
    // The alpha component is always set to full (255).
    function toColorStruct(color) {
        switch (color)
        {
        // Ink colors
        case "Black":
            return Windows.UI.Colors.black;
        case "Blue":
            return Windows.UI.Colors.blue;
        case "Red":
            return Windows.UI.Colors.red;
        case "Green":
            return Windows.UI.Colors.green;

        // Highlighting colors
        case "Yellow":
            return Windows.UI.Colors.yellow;
        case "Aqua":
            return Windows.UI.Colors.aqua;
        case "Lime":
            return Windows.UI.Colors.lime;

        // Select colors
        case "Gold":
            return Windows.UI.Colors.gold;

        case "White":
            return Windows.UI.Colors.white;
        }

        if ((color.length === 7) && (color.charAt(0) === "#")) {
            var r = parseInt(color.substr(1, 2), 16);
            var g = parseInt(color.substr(3, 2), 16);
            var b = parseInt(color.substr(5, 2), 16);
            return Windows.UI.ColorHelper.fromArgb(255, r, g, b);
        }

        return Windows.UI.Colors.gray;
    }

    // Global variable representing the application.
    var app;

    // Global variables representing the ink interface.
    // The usage of a global variable for drawingAttributes is not completely necessary,
    // just a convenience.  One could always re-fetch the current drawingAttributes
    // from the inkManager.
    var inkManager = new Windows.UI.Input.Inking.InkManager();
    var drawingAttributes = new Windows.UI.Input.Inking.InkDrawingAttributes();
    drawingAttributes.fitToCurve = true;
    inkManager.setDefaultDrawingAttributes(drawingAttributes);

    // These are the global canvases (and their 2D contexts) for highlighting, for drawing ink,
    // and for lassoing (and erasing).
    var hlCanvas;
    var hlContext;
    var inkCanvas;
    var inkContext;
    var selCanvas;
    var selContext;

    // The "mode" of whether we are highlighting, inking, lassoing, or erasing is controlled by this global variable,
    // which should be pointing to either hlContext, inkContext, or selContext.
    // In lassoing mode (when context points to selContext), we might also be in erasing mode;
    // the state of lassoing vs. erasing is kept inside the ink manager, in attribute "mode", which will
    // have a value from enum Windows.UI.Input.Inking.InkManipulationMode, one of either "selecting"
    // or "erasing" (the other value being "inking" but in that case context will be pointing to one of the other
    // 2 canvases).
    var context;

    // Three functions to save and restore the current mode, and to clear this state.

    // Note that we can get into erasing mode in one of two ways: there is a eraser button in the toolbar,
    // and some pens have an active back end that is meant to represent erasing.  If we get into erasing
    // mode via the button, we stay in that mode until another button is pushed.  If we get into erasing
    // mode via the eraser end of the stylus, we should switch out of it when the user switches to the ink
    // end of the stylus.  And we want to return to the mode we were in before this happened.  Thus we
    // maintain a shallow stack (depth 1) of "mode" info.

    var savedContext = null;
    var savedStyle = null;
    var savedCursor = null;
    var savedMode = null;

    function clearMode() {
        savedContext = null;
        savedStyle = null;
        savedCursor = null;
        savedMode = null;
    }

    function saveMode() {
        if (!savedContext) {
            savedStyle = context.strokeStyle;
            savedContext = context;
            savedCursor = selCanvas.style.cursor;
            savedMode = inkManager.mode;
        }
    }

    function restoreMode() {
        if (savedContext) {
            context = savedContext;
            context.strokeStyle = savedStyle;
            inkManager.mode = savedMode;
            selCanvas.style.cursor = savedCursor;
            clearMode();
        }
    }

    // Global variable representing the pattern used when in select mode.  This is an 8*1 image with 4 bits set,
    // then 4 bits cleared, to give us a dashed line when drawing a lasso.
    var selPattern;

    // Global pointer to find flyout.
    var findFlyout;

    // Global pointer to the flyout used for displaying recognition results (top 5 alternates),
    // and an array of buttons (one per alternate).
    var recoFlyout;
    var clipButtons;

    // Global pointer to the invisible <div> that marks the location of the currently selected word.
    var wordDiv;

    // Global pointer to the text buffer inside the Find flyout.
    var findText;

    // Returns true if any strokes inside the ink manager are selected; false otherwise.
    function anySelected() {
        return inkManager.getStrokes().some(function (stroke) {
            return stroke.selected;
        });
    }

    // Makes all strokes a part of the selection.
    function selectAll() {
        inkManager.getStrokes().forEach(function (stroke) {
            stroke.selected = true;
        });
    }

    // Makes all non-highlight strokes a part of the selection.
    function selectAllNoHighlight() {
        inkManager.getStrokes().forEach(function (stroke) {
            if (!stroke.drawingAttributes.drawAsHighlighter) {
                stroke.selected = true;
            }
        });
    }

    // Unselects any strokes which are highlighting.
    function unselectHighlight() {
        inkManager.getStrokes().forEach(function (stroke) {
            if (stroke.selected && stroke.drawingAttributes.drawAsHighlighter) {
                stroke.selected = false;
            }
        });
    }

    // Returns true if the point represented by x,y is within the rect.
    function inRect(x, y, rect) {
        return ((rect.x <= x) && (x < (rect.x + rect.width)) &&
                (rect.y <= y) && (y < (rect.y + rect.height)));
    }

    // Tests the array of results bounding boxes (from the recognition results on the ink manager).
    // Returns an object representing the results, with the original touch coordinates, the bounding
    // box, the index of the result, the array of strokes, and the array of alternates (recognition strings).
    // If recognition has not run or touch is outside of any word bounding box, then returns null.
    function hitTest(tx, ty) {
        var results = inkManager.getRecognitionResults();
        var cWords = results.size;

        if (cWords === 0) {
            return null;
        }

        for (var i = 0; i < cWords; i++) {
            var rect = results[i].boundingRect;
            if (inRect(tx, ty, rect)) {
                return {
                    index: i,
                    handleX: tx,  // Original touch point
                    handleY: ty,
                    strokes: results[i].getStrokes(),
                    rect: rect,
                    alternates: results[i].getTextCandidates()
                };
            }
        }
        return null;
    }

    // Note that we cannot just set the width in stroke.drawingAttributes.size.width,
    // or the color in stroke.drawingAttributes.color.
    // The stroke API supports get and put operations for drawingAttributes,
    // but we must execute those operations separately, and change any values
    // inside drawingAttributes between those operations.

    // Change the color and width in the default (used for new strokes) to the values
    // currently set in the current context.
    function setDefaults() {
        var strokeSize = drawingAttributes.size;
        strokeSize.width = strokeSize.height = context.lineWidth;
        drawingAttributes.size = strokeSize;
        drawingAttributes.color = toColorStruct(context.strokeStyle);
        drawingAttributes.drawAsHighlighter = context === hlContext;
        inkManager.setDefaultDrawingAttributes(drawingAttributes);
    }

    // Four functions to switch back and forth between ink mode, highlight mode, select mode, and erase mode.
    // There is also a temp erase mode, which uses the saveMode()/restoreMode() functions to
    // return us to our previous mode when done erasing.  This is used for quick erasers using the back end
    // of the pen (for those pens that have that).
    // NOTE: The erase modes also attempt to set the mouse/pen cursor to the image of a chalkboard eraser
    // (stored in images/erase.cur), but as of this writing cursor switching is not working.

    function highlightMode() {
        clearMode();
        context = hlContext;
        inkManager.mode = Windows.UI.Input.Inking.InkManipulationMode.inking;
        setDefaults();
        selCanvas.style.cursor = "default";
    }

    function inkMode() {
        clearMode();
        context = inkContext;
        inkManager.mode = Windows.UI.Input.Inking.InkManipulationMode.inking;
        setDefaults();
        selCanvas.style.cursor = "default";
    }

    function selectMode() {
        clearMode();
        selContext.strokeStyle = selPattern;
        context = selContext;
        inkManager.mode = Windows.UI.Input.Inking.InkManipulationMode.selecting;
        selCanvas.style.cursor = "default";
    }

    function eraseMode() {
        clearMode();
        selContext.strokeStyle = "rgba(255,255,255,0.0)";
        context = selContext;
        inkManager.mode = Windows.UI.Input.Inking.InkManipulationMode.erasing;
        selCanvas.style.cursor = "url(images/erase.cur), auto";
    }

    function tempEraseMode() {
        saveMode();
        selContext.strokeStyle = "rgba(255,255,255,0.0)";
        context = selContext;
        inkManager.mode = inkManager.mode = Windows.UI.Input.Inking.InkManipulationMode.erasing;
        selCanvas.style.cursor = "url(images/erase.cur), auto";
    }

    // Set the width of a stroke.  Return true if we actually changed it.
    // Note that we cannot just set the width in stroke.drawingAttributes.size.width.
    // The stroke API supports get and put operations for drawingAttributes,
    // but we must execute those operations separately, and change any values
    // inside drawingAttributes between those operations.
    function shapeStroke(stroke, width) {
        var att = stroke.drawingAttributes;
        var strokeSize = att.size;
        if (strokeSize.width !== width) {
            strokeSize.width = strokeSize.height = width;
            att.size = strokeSize;
            stroke.drawingAttributes = att;
            return true;
        } else {
            return false;
        }
    }

    // Set the color (and alpha) of a stroke.  Return true if we actually changed it.
    // Note that we cannot just set the color in stroke.drawingAttributes.color.
    // The stroke API supports get and put operations for drawingAttributes,
    // but we must execute those operations separately, and change any values
    // inside drawingAttributes between those operations.
    function colorStroke(stroke, color) {
        var att = stroke.drawingAttributes;
        var clr = toColorStruct(color);
        if (att.color !== clr) {
            att.color = clr;
            stroke.drawingAttributes = att;
            return true;
        } else {
            return false;
        }
    }

    // Global memory of the current pointID (for pen, and, separately, for touch).
    // We ignore handlePointerMove() and handlePointerUp() calls that don't use the same
    // pointID as the most recent handlePointerDown() call.  This is because the user sometimes
    // accidentally nudges the mouse while inking or touching.  This can cause move events
    // for that mouse that have different x,y coordinates than the ink trace or touch path
    // we are currently handling.

    // 'Pointer*' events maintain this pointId so that one can track individual fingers,
    // the pen, and the mouse.

    // Note that when the pen fails to leave the area where it can be sensed, it does NOT
    // get a new ID; so it is possible for 2 or more consecutive strokes to have the same ID.

    var penID = -1;

    // This global variable holds a reference to the div that is imposed on top of selected ink.
    // It is used to register event handlers that allow the user to move around selected ink.
    var selBox;

    // Hides the (transparent) div that is used to capture events for moving selected ink
    function anchorSelection() {
        // Make selBox of size 0 and move it to the top-left corner
        selBox.style.left = "0px";
        selBox.style.top = "0px";
        selBox.style.width = "0px";
        selBox.style.height = "0px";
    }

    // Places the (transparent) div that is used to capture events for moving selected ink.
    // The assumption is that rect is the bounding box of the selected ink.
    function detachSelection(rect) {
        // Move and resize selBox so that it perfectly overlaps with rect
        selBox.rect = rect;
        selBox.style.left = selBox.rect.x + "px";
        selBox.style.top = selBox.rect.y + "px";
        selBox.style.width = selBox.rect.width + "px";
        selBox.style.height = selBox.rect.height + "px";
    }

    // We will accept pen down or mouse left down as the start of a stroke.
    // We will accept touch down or mouse right down as the start of a touch.
    function handlePointerDown(evt) {
        if ((evt.pointerType === "pen") || ((evt.pointerType === "mouse") && (evt.button === 0))) {
            // Anchor and clear any current selection.
            anchorSelection();
            var pt = {x:0.0, y:0.0};
            inkManager.selectWithLine(pt, pt);

            pt = evt.currentPoint;

            if (pt.properties.isEraser) { // The back side of a pen, which we treat as an eraser
                tempEraseMode();
            } else {
                restoreMode();
            }

            context.beginPath();
            context.moveTo(pt.rawPosition.x, pt.rawPosition.y);

            inkManager.processPointerDown(pt);
            penID = evt.pointerId;
        } else if (evt.pointerType === "touch") {
            // Start the processing of events related to this pointer as part of a gesture.
            // In this sample we are interested in MSGestureTap event, which we use to show alternates. See handleTap event handler. 
            selCanvas.gestureObject.addPointer(evt.pointerId);
        }
    }

    function handlePointerMove(evt) {
        if (evt.pointerId === penID) {
            var pt = evt.currentPoint;
            context.lineTo(pt.rawPosition.x, pt.rawPosition.y);
            context.stroke();
            // Get all the points we missed and feed them to inkManager.
            // The array pts has the oldest point in position length-1; the most recent point is in position 0.
            // Actually, the point in position 0 is the same as the point in pt above (returned by evt.currentPoint).
            var pts = evt.intermediatePoints;
            for (var i = pts.length - 1; i >= 0 ; i--) {
                inkManager.processPointerUpdate(pts[i]);
            }
        }

        // No need to process touch events - selCanvas.gestureObject takes care of them and triggers MSGesture* events.
    }

    function handlePointerUp(evt) {
        if (evt.pointerId === penID) {
            penID = -1;
            var pt = evt.currentPoint;
            context.lineTo(pt.rawPosition.x, pt.rawPosition.y);
            context.stroke();
            context.closePath();

            var rect = inkManager.processPointerUp(pt);
            if (inkManager.mode === Windows.UI.Input.Inking.InkManipulationMode.selecting) {
                detachSelection(rect);
            }

            renderAllStrokes();
        }
    }

    // We treat the event of the pen leaving the canvas as the same as the pen lifting;
    // it completes the stroke.
    function handlePointerOut(evt) {
        if (evt.pointerId === penID) {
            var pt = evt.currentPoint;
            context.lineTo(pt.rawPosition.x, pt.rawPosition.y);
            context.stroke();
            context.closePath();
            inkManager.processPointerUp(pt);
            penID = -1;
            renderAllStrokes();
        }
    }

    function handleTap(evt) {
        // Anchor and clear any current selection.
        if (anySelected()) {
            anchorSelection();
            var pt = {x:0.0, y:0.0};
            inkManager.selectWithLine(pt, pt);
            renderAllStrokes();
        }

        var touchedResults = hitTest(evt.offsetX, evt.offsetY);
        if (touchedResults) {
            touchWord(touchedResults);
        }
    }

    function handleSelectionBoxPointerDown(evt) {
        // Start the processing of events related to this pointer as part of a gesture.
        // In this sample we are interested in MSGestureChange event, which we use to move selected ink.
        // See handleSelectionBoxGestureChange event handler.
        selBox.gestureObject.addPointer(evt.pointerId);
    }

    function handleSelectionBoxGestureChange(evt) {
        // Move selection box
        selBox.rect.x += evt.translationX;
        selBox.rect.y += evt.translationY;
        selBox.style.left = selBox.rect.x + "px";
        selBox.style.top = selBox.rect.y + "px";

        // Move selected ink
        inkManager.moveSelected({x: evt.translationX, y: evt.translationY});

        renderAllStrokes();
    }

    // Draws a single stroke into a specified canvas 2D context, with a specified color and width.
    function renderStroke(stroke, color, width, ctx) {
        ctx.save();

        ctx.beginPath();
        ctx.strokeStyle = color;
        ctx.lineWidth = width;

        var first = true;
        stroke.getRenderingSegments().forEach(function (segment) {
            if (first) {
                ctx.moveTo(segment.position.x, segment.position.y);
                first = false;
            } else {
                ctx.bezierCurveTo(segment.bezierControlPoint1.x, segment.bezierControlPoint1.y,
                                    segment.bezierControlPoint2.x, segment.bezierControlPoint2.y,
                                    segment.position.x,            segment.position.y);
            }
        });

        ctx.stroke(); 
        ctx.closePath();

        ctx.restore();
    }

    // This draws a basic notepaper pattern into the highlight canvas, which is the lowest canvas.
    // It has a single vertical dark red line defining the left margin, and a series of horizontal blue lines.
    function renderPaper() {
        var height = hlCanvas.height;
        var bottom = height - 0.5;
        var right = hlCanvas.width - 0.5;

        hlContext.save();

        hlContext.beginPath();
        hlContext.strokeStyle = "FireBrick";
        hlContext.lineWidth = 1;
        hlContext.moveTo(120.5, 0.5);
        hlContext.lineTo(120.5, bottom);
        hlContext.stroke();
        hlContext.closePath();

        hlContext.beginPath();
        hlContext.strokeStyle = "Blue";
        hlContext.lineWidth = 1;
        for (var y = 65.5; y < height; y += 55) {
            hlContext.moveTo(0.5, y);
            hlContext.lineTo(right, y);
        }
        hlContext.stroke();
        hlContext.closePath();

        hlContext.restore();
    }

    // Redraws (from the beginning) all strokes in the canvases.  All canvases are erased,
    // then the paper is drawn, then all the strokes are drawn.
    function renderAllStrokes() {
        selContext.clearRect(0, 0, selCanvas.width, selCanvas.height);
        inkContext.clearRect(0, 0, inkCanvas.width, inkCanvas.height);
        hlContext.clearRect(0, 0, hlCanvas.width, hlCanvas.height);

        renderPaper();

        inkManager.getStrokes().forEach(function (stroke) {
            var att = stroke.drawingAttributes;
            var color = toColorString(att.color);
            var strokeSize = att.size;
            var width = strokeSize.width;
            var hl = stroke.drawingAttributes.drawAsHighlighter;
            var ctx = hl ? hlContext : inkContext;
            if (stroke.selected) {
                renderStroke(stroke, color, width * 2, ctx);
                var stripe = hl ? "Azure" : "White";
                var w = width - (hl ? 3 : 1);
                renderStroke(stroke, stripe, w, ctx);
            } else {
                renderStroke(stroke, color, width, ctx);
            }
        });
    }

    function clear() {
        if (anySelected()) {
            inkManager.deleteSelected();
        } else {
            selectAll();
            inkManager.deleteSelected();
            inkMode();
        }

        renderAllStrokes();
        displayStatus("");
        displayError("");
    }

    // A generic function for use for any async error function (the second arg to a then() method).
    function asyncError(e) {
        displayError("Async error: " + e.toString());
    }

    function refresh() {
        renderAllStrokes();
    }

    // A button handler which fetches the ID from the button, which should
    // be IW2, IW4, etc.  We set the lineWidth of the inking canvas to the number part of this ID,
    // then set the system into ink mode (which will cause the ink manager
    // to change its defaults for new strokes to match the ink canvas).
    // If any ink strokes (not including highlight strokes) are currently selected,
    // we also change their width to this value.  If any strokes are changed
    // we must re-render the entire ink display.
    function setInkWidth(evt) {
        var id = evt.currentTarget.id;
        id = id.substr(2);
        inkContext.lineWidth = id;
        inkMode();

        var redraw = false;
        inkManager.getStrokes().forEach(function (stroke) {
            if (stroke.selected && !stroke.drawingAttributes.drawAsHighlighter) {
                if (shapeStroke(stroke, inkContext.lineWidth)) {
                    redraw = true;
                }
            }
        });
        if (redraw) {
            renderAllStrokes();
        }
    } 

    // A button handler which fetches the ID from the button, which should
    // be HW10, HW20, or HW30.  We set the lineWidth of the highlighting canvas to the number part of this ID,
    // then set the system into highlight mode (which will cause the ink manager
    // to change its defaults for new strokes to match the highlight canvas).
    // If any highlight strokes are currently selected, we also change their width
    // to this value.  If any strokes are changed we must re-render the dirty areas.
    function setHighlightWidth(evt) {
        var id = evt.currentTarget.id;
        id = id.substr(2);
        hlContext.lineWidth = id;
        highlightMode();

        var redraw = false;
        inkManager.getStrokes().forEach(function (stroke) {
            if (stroke.selected && stroke.drawingAttributes.drawAsHighlighter) {
                if (shapeStroke(stroke, hlContext.lineWidth)) {
                    redraw = true;
                }
            }
        });
        if (redraw) {
            renderAllStrokes();
        }
    } 

    // A button handler which fetches the ID from the button, which should
    // be a color name.  We set the strokeStyle of the inking canvas to this color,
    // then set the system into ink mode (which will cause the ink manager
    // to change its defaults for new strokes to match the ink canvas).
    // If any ink strokes (not including highlight strokes) are currently selected,
    // we also change their color to this value.  If any strokes are changed
    // we must re-render the dirty areas.
    function inkColor(evt) {
        inkContext.strokeStyle = evt.currentTarget.id;
        inkMode();

        var redraw = false;
        inkManager.getStrokes().forEach(function (stroke) {
            if (stroke.selected && !stroke.drawingAttributes.drawAsHighlighter) {
                if (colorStroke(stroke, inkContext.strokeStyle)) {
                    redraw = true;
                }
            }
        });
        if (redraw) {
            renderAllStrokes();
        }
    }

    // A button handler which fetches the ID from the button, which should
    // be a color name.  We set the strokeStyle of the highlight canvas to this color,
    // then set the system into highlight mode (which will cause the ink manager
    // to change its defaults for new strokes to match the highlight canvas).
    // If any highlight strokes are currently selected, we also change their color
    // to this value.  If any strokes are changed we must re-render the dirty areas.
    function highlightColor(evt) {
        hlContext.strokeStyle = evt.currentTarget.id.substr(2);
        highlightMode();

        var redraw = false;
        inkManager.getStrokes().forEach(function (stroke) {
            if (stroke.selected && stroke.drawingAttributes.drawAsHighlighter) {
                if (colorStroke(stroke, hlContext.strokeStyle)) {
                    redraw = true;
                }
            }
        });
        if (redraw) {
            renderAllStrokes();
        }
    }

    // Finds a specific recognizer, and sets the inkManager's default to that recognizer.
    // Returns true if successful.
    function setRecognizerByName(recoName) {
        // 'recognizers' is a normal JavaScript array
        var recognizers = inkManager.getRecognizers();
        for (var i = 0, len = recognizers.length; i < len; i++) {
            if (recoName === recognizers[i].name) {
                inkManager.setDefaultRecognizer(recognizers[i]);
                return true;
            }
        }
        return false;
    }

    // Prevent two concurrent recognizeAsync() operations
    var recognizeOperationRunning = false;

    // A button handler which runs the currently-loaded handwriting recognizer over
    // the selected ink (not counting highlight strokes).  If no ink is selected, then it
    // runs over all the ink (again, not counting highlight strokes).
    // The recognition results (a string) is displayed in the status window.
    // The recognition results are also stored within the ink manager itself, so that
    // other commands can find the bounding boxes (or ink strokes) of any specific
    // word of ink.
    function recognize(evt) {
        if (recognizeOperationRunning) {
            return;
        }
        // The recognizeAsync() method has 3 modes: selected, remaining, and all.
        // This particular app cannot use "all" mode because it supports highlighting.
        // If the user has highlighted one or more words, and we recognize in "all" mode,
        // we will recognize all strokes, including the highlight strokes.  This usually
        // results in a recognition string containing many asterisks.
        // If we find that no strokes are selected, rather than running in "all" mode, we
        // select all strokes that are not highlighting strokes, then run in "selected" mode.
        // If some strokes were already selected, we just need to unselect any which are highlighting.

        // If we DID originally find that no strokes were selected, we remember that fact, so that
        // we can unselect them after the recognition.
        var bSelected = false;
        if (anySelected()) {
            unselectHighlight();
        } else {
            selectAllNoHighlight();
            bSelected = true;
        }

        // NOTE: check that we have some ink to recognize before calling InkManager::RecognizeAsync()
        if (anySelected()) {
            // About to call recognizeAsync()
            // Prevent future calls to this API until we are done with the first call
            recognizeOperationRunning = true;

            // Note that the third mode in recognizeAsync(), "recent", can be very useful in certain situations,
            // but we are not using it here.  It will recognize all strokes that have been added since the last
            // recognition.  If we were assuming that all strokes were writing, and we were trying to keep
            // recognition caught up with the user's writing at all times (that is, not using a Reco button),
            // then "recent" would be the mode we would want.

            // Because recognition is slower, we ask for it as an asynchronous operation.
            // The anonymous function (the first arg to the "then" method) will be called
            // as a callback when recognition has completed.  If an error occurs, the second
            // arg will be called.
            inkManager.recognizeAsync(Windows.UI.Input.Inking.InkRecognitionTarget.selected).done(
                function (results) {
                    // Doing a recognition does not update the storage of results (the results that are stored inside the ink manager).
                    // We do that ourselves by calling this method.
                    inkManager.updateRecognitionResults(results);

                    // The arg "results" is an array of result objects representing "words", where "words" means words of ink (not computer memory words).
                    // I.e., if you write "this is a test" that is 4 words, and results will be an array of length 4.

                    var alternates = ""; // Will accumulate the result words, with spaces between
                    results.forEach(function (result) {
                        // Method getTextCandidates() returns an array of recognition alternates (different interpretations of the same word of ink).
                        // This is a standard JavaScript array of standard JavaScript strings.
                        // For this program we only use the first (top) alternate in our display.
                        // If we were doing search over this ink, we would want to search all alternates.
                        var alts = result.getTextCandidates();
                        alternates = alternates + " " + alts[0];

                        // The specific strokes forming the current word of ink are available to us.
                        // This feature is not used here, but we could, if we chose, display the ink,
                        // with the recognition result for each word directly above the specific word of ink,
                        // by fetching the bounding box of the recognitionResult (via the boundingRect property).
                        // Or, if we needed to do something to each stroke in the recognized word, we could
                        // call recognitionResult.getStrokes(), then iterate over the individual strokes.
                    });
                    displayStatus(alternates);

                    // Reset recognizeOperationRunning, can call recognizeAsync() once again
                    recognizeOperationRunning = false;
                },
                function (e) {
                    displayError("InkManager::recognizeAsync: " + e.toString());

                    // We still want to reset recognizeOperationRunning if an error occurs
                    recognizeOperationRunning = false;
                }
            );
            if (bSelected) {
                // Unselect all strokes (if we originally had no selected strokes).
                var pt = {x:0.0, y:0.0};
                inkManager.selectWithLine(pt, pt);
            }
        } else {
            displayStatus("Must first write something.");
        }
    }

    // A utility function for findText() below.  This takes a target string (typed in by the user)
    // an an array of recognition results objects, and inspects the recognition alternates of each
    // results object.  If a match is found among the alternates, then all strokes in that results
    // object are selected.  The match is case-insensitive.
    function findWord(target, results) {
        target = target.toLowerCase();

        var count = 0;
        results.forEach(function (result) {
            var alternates = result.getTextCandidates();
            var cAlts = alternates.length;
            for (var j = 0; j < cAlts; j++) {
                if (alternates[j].toLowerCase() === target) {
                    result.getStrokes().forEach(function (stroke) {
                        stroke.selected = true;
                    });
                    count++;
                    break;
                }
            }
        });
        return count;
    }

    // A handler for the Find button in the Find flyout.  We fetch the search string
    // from the form, and the array of recognition results objects from the ink
    // manager.  We unselect any current selection, so that when we are done
    // the selections will reflect the search results.  We split the search string into
    // individual words, since our recognition results objects each represent individual
    // words.  The actual matching is done by findWord(), defined above.

    // Note that multiple instances of a target can be found; if the target is "this" and
    // the ink contains "this is this is that", 2 instances of "this" will be found and all
    // strokes in both words will be selected.

    // Note that findWord() above searches all alternates.  This means you might write
    // "this", have it mis-recognized as "these", but the search feature MAY find it, if
    // "this" appears in any of the other 4 recognition alternates for this ink.
    function find(evt) {
        var str = findText.value;
        var results = inkManager.getRecognitionResults();

        // This will unselect any current selection.
        var pt = {x:0.0, y:0.0};
        inkManager.selectWithLine(pt, pt); 

        var count = 0;
        str.split(" ").forEach(function (word) {
            count += findWord(word, results);
        });

        if (0 < count) {
            displayStatus("Found " + /*@static_cast(String)*/count + " words");
            renderAllStrokes();
        } else {
            displayStatus("Did not find " + str);
        }
        return false;
    }

    // A button click handler for recognition results buttons in the "reco" Flyout.
    // The flyout shows the top 5 recognition results for a specific word, and
    // is invoked by tapping (with finger) on a word (after recognition has been run).
    // We fetch the recognition result (the innerHTML of the button, a string) and
    // copy it to the clipboard.
    function recoClipboard(evt) {
        recoFlyout.winControl.hide();
        var alt = evt.currentTarget.winControl.label;
        
        var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();
        dataPackage.setText(alt);
        Windows.ApplicationModel.DataTransfer.Clipboard.setContent(dataPackage);
        displayStatus("To clipboard: " + alt);
    }

    // Brings up the "reco" Flyout, after first changing the values of the 5 buttons to be
    // the top 5 recognition alternates of a single word.
    function touchWord(touchedResults) {
        // The Windows.UI.Input.Inking.InkManager interface normally returns 5 alternates.
        // We check just to be sure we are not given more alternates than the count of buttons.
        var cAlts = touchedResults.alternates.size;
        if (cAlts === 0) {
            return;
        }
        var cButs = clipButtons.length;
        if (cButs < cAlts) {
            cAlts = cButs;
        }
        var i;
        for (i = 0; i < cAlts; i++) {
            clipButtons[i].label = touchedResults.alternates[i];
        }
        for (; i < cButs; i++) {
            clipButtons[i].label = "";
        }

        // Display the reco results menu just to the left of the left-top corner of the bounding rect of the ink.
        var rect = touchedResults.rect;
        wordDiv.style.left = /*@static_cast(String)*/rect.x + "px";
        wordDiv.style.top = /*@static_cast(String)*/rect.y + "px";
        wordDiv.style.width = /*@static_cast(String)*/rect.width + "px";
        wordDiv.style.height = /*@static_cast(String)*/rect.height + "px";
        recoFlyout.winControl.show(wordDiv, "left");
    }

    // A button handler which copies the selected strokes (or all the strokes if none are selected)
    // into the clipboard.  The strokes can be pasted into any application that handles any of the
    // ink clipboard formats, such as Windows Journal.
    function copySelected(evt) {
        if (anySelected()) {
            displayStatus("Copying selected strokes ...");
            inkManager.copySelectedToClipboard();
            displayStatus("Copy Selected");
        } else {
            displayStatus("Copying all strokes ...");
            selectAll();
            inkManager.copySelectedToClipboard();
            // Unselect all strokes.
            var pt = {x:0.0, y:0.0};
            inkManager.selectWithLine(pt, pt); 
            displayStatus("Copy All");
        }
    }

    // A button handler which copies any available strokes in the clipboard into this app.
    function paste(evt) {
        displayStatus("Pasting ...");
        var insertionPoint = {x: 100, y: 60};
        var canPaste = inkManager.canPasteFromClipboard();
        if (canPaste) {
            inkManager.pasteFromClipboard(insertionPoint);
            displayStatus("Pasted");
            renderAllStrokes();
        } else {
            displayStatus("Cannot paste");
        }
    }

    // A keypress handler which closes the program.
    // A normal program should not have this, but it is very
    // convenient for testing.
    function closeProgram(evt) {
        displayStatus("Closing App ...");
        window.close();
    }

    // Prevent two concurrent loadAsync() operations
    var openOperationRunning = false;

    // Reads a gif file which contains strokes as metadata.
    function readInk(storageFile) {
        if (storageFile) {
            // Closure variable, visible to all promises in the following chain
            var loadStream = null;
            storageFile.openAsync(Windows.Storage.FileAccessMode.read).then(
                function (stream) {
                    // About to call loadAsync()
                    // Prevent future calls to this API until we are done with the first call
                    openOperationRunning = true;
                    loadStream = stream;
                    return inkManager.loadAsync(loadStream); // Since we return the promise, it will be executed before the following .done
                }
            ).done(
                function () {
                    var strokes = inkManager.getStrokes();
                    var c = strokes.length;
                    if (c === 0) {
                        displayStatus("File does not contain any ink strokes.");
                    } else {
                        displayStatus("Loaded " + c + " strokes.");
                        renderAllStrokes();
                    }

                    // Reset openOperationRunning, can call loadAsync() once again
                    openOperationRunning = false;

                    // Input stream is IClosable interface and requires explicit close
                    loadStream.close();
                },
                function (e) {
                    displayError("Load failed. Make sure you tried to open a file that can be read by the InkManager.");

                    // We still want to reset openOperationRunning if an error occurs
                    openOperationRunning = false;

                    // If the error occurred after the stream was opened, close the stream
                    if (loadStream) {
                        loadStream.close();
                    }
                }
            );
        }
    }

    // A button handler which fetches the file name via the file picker, then calls readInk() above.
    function load(evt) {
        if (openOperationRunning) {
            return;
        }

        // Open the file picker, set the input folder, and set the input extension.
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        picker.fileTypeFilter.replaceAll([".gif"]);
        picker.pickSingleFileAsync().done(readInk, asyncError);
    }

    function writeInk(storageFile) {
        if (storageFile) {
            // Closure variable, visible to all promises in the following chain
            var saveStream = null;
            storageFile.openAsync(Windows.Storage.FileAccessMode.readWrite).then(
                function (stream) {
                    saveStream = stream;
                    return inkManager.saveAsync(saveStream); // Since we return the promise, it will be executed before the following .then
                }
            ).done(
                function (result) {
                    // Print the size of the stream on the screen
                    displayStatus("File saved!");

                    // Output stream is IClosable interface and requires explicit close
                    saveStream.close();
                },
                function (e) {
                    displayError("Save " + e.toString());

                    // If the error occurred after the stream was opened, close the stream
                    if (saveStream) {
                        saveStream.close();
                    }
                }
            );
        }
    }

    // Shows the create file dialog box. Submitting on that form will invoke saveFile() above.
    function save(evt) {
        // NOTE: make sure that the inkManager has some strokes to save before calling inkManager.saveAsync
        if (inkManager.getStrokes().size > 0) {
            var picker = new Windows.Storage.Pickers.FileSavePicker();
            picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
            picker.fileTypeChoices.insert("GIF file", [".gif"]);
            picker.defaultFileExtension = ".gif";
            picker.pickSaveFileAsync().done(writeInk, asyncError);
        } else {
            displayStatus("The InkManager doesn't contain any strokes to save.");
        }
    }

    // A keypress handler that only handles a few keys.  This is registered on the entire body.
    // Escape will:
    //   1. If any dialog boxes are showing, hide them and do nothing else.
    //   2. Otherwise, if any strokes are selected, unselect them and do nothing else.
    //   3. Otherwise, change to ink mode.
    // This sequence allows us to "unpeel the onion" (it is very fast to hit escape 3 times if needed).

    // Certain control keys invoke handlers that are otherwise invoked via buttons:
    //   ^C  Copy
    //   ^V  Paste
    //   ^F  Find
    //   ^O  Load
    //   ^S  Save
    //   ^R  Recognize
    //   ^Q  Quit (shuts down the sample app)

    // Note that most of these keys have standardized normal uses, and there is system code to handle that
    // without our code doing anything.  That code sometimes interferes with our program.  All the functions
    // we call from here call evt.preventDefault(), which should stop the default processing, but sometimes we still
    // cannot get this code to execute.
    function keypress(evt) {
        if (evt.keyCode === 27) { // Escape
            evt.preventDefault();
            if (!recoFlyout.winControl.hidden) {
                recoFlyout.winControl.hide();
                renderAllStrokes();
            } else if (anySelected()) {
                // Unselect all strokes.
                var pt = {x:0.0, y:0.0};
                inkManager.selectWithLine(pt, pt); 
                renderAllStrokes();
            } else {
                inkMode();
            }
        } else if (evt.keyCode === 3) { // Control c
            copySelected(evt);
        } else if (evt.keyCode === 22) { // Control v
            paste(evt);
        } else if (evt.keyCode === 15) { // Control o
            load(evt);
        } else if (evt.keyCode === 19) { // Control s
            save(evt);
        } else if (evt.keyCode === 18) { // Control r
            recognize(evt);
        } else if (evt.keyCode === 17) { // Control q
            closeProgram(evt);
        }
    }

    function inkInitialize() {
        // Utility to fetch elements by ID.
        function id(elementId) {
            return document.getElementById(elementId);
        }

        WinJS.UI.processAll().then(
            function () {
                id("Black").addEventListener("click", inkColor);
                id("Blue").addEventListener("click", inkColor);
                id("Red").addEventListener("click", inkColor);
                id("Green").addEventListener("click", inkColor);
                id("IW2").addEventListener("click", setInkWidth);
                id("IW4").addEventListener("click", setInkWidth);
                id("IW6").addEventListener("click", setInkWidth);
                id("IW8").addEventListener("click", setInkWidth);
                id("IW10").addEventListener("click", setInkWidth);
                id("HLYellow").addEventListener("click", highlightColor);
                id("HLAqua").addEventListener("click", highlightColor);
                id("HLLime").addEventListener("click", highlightColor);
                id("HW10").addEventListener("click", setHighlightWidth);
                id("HW20").addEventListener("click", setHighlightWidth);
                id("HW30").addEventListener("click", setHighlightWidth);

                findFlyout = id("FindFlyout");

                findText = id("FindString");
                findFlyout.addEventListener("aftershow", function (evt) { findText.focus(); }, false);
                id("FindButton").addEventListener("click", find, false);

                hlCanvas = id("HighlightCanvas");
                hlCanvas.setAttribute("width", hlCanvas.offsetWidth);
                hlCanvas.setAttribute("height", hlCanvas.offsetHeight);
                hlContext = hlCanvas.getContext("2d");
                hlContext.lineWidth = 10;
                hlContext.strokeStyle = "Yellow";
                hlContext.lineCap = "round";
                hlContext.lineJoin = "round";

                inkCanvas = id("InkCanvas");
                inkCanvas.setAttribute("width", inkCanvas.offsetWidth);
                inkCanvas.setAttribute("height", inkCanvas.offsetHeight);
                inkContext = inkCanvas.getContext("2d");
                inkContext.lineWidth = 2;
                inkContext.strokeStyle = "Black";
                inkContext.lineCap = "round";
                inkContext.lineJoin = "round";

                selCanvas = id("SelectCanvas");
                selCanvas.gestureObject = new MSGesture();
                selCanvas.gestureObject.target = selCanvas;
                selCanvas.setAttribute("width", selCanvas.offsetWidth);
                selCanvas.setAttribute("height", selCanvas.offsetHeight);
                selContext = selCanvas.getContext("2d");
                selContext.lineWidth = 1;
                selContext.strokeStyle = "Gold";
                selContext.lineCap = "round";
                selContext.lineJoin = "round";

                selBox = id("SelectionBox");
                selBox.addEventListener("pointerdown", handleSelectionBoxPointerDown, false);
                selBox.addEventListener("MSGestureChange", handleSelectionBoxGestureChange, false);
                selBox.gestureObject = new MSGesture();
                selBox.gestureObject.target = selBox;
                selBox.style.left = "0px";
                selBox.style.top = "0px";
                selBox.style.width = "0px";
                selBox.style.height = "0px";

                // Note that we must set the event listeners on the top-most canvas.

                selCanvas.addEventListener("pointerdown", handlePointerDown, false);
                selCanvas.addEventListener("pointerup", handlePointerUp, false);
                selCanvas.addEventListener("pointermove", handlePointerMove, false);
                selCanvas.addEventListener("pointerout", handlePointerOut, false);
                selCanvas.addEventListener("MSGestureTap", handleTap, false);

                var image = new Image();
                image.onload = function () { selContext.strokeStyle = selPattern = selContext.createPattern(image, "repeat"); };
                image.src = "images/select.png";

                recoFlyout = id("RecoFlyout");
                clipButtons = new Array();
                for (var i = 0; i < 5; i++) {
                    var ID = "Reco" + i;
                    clipButtons[i] = recoFlyout.winControl.getCommandById(ID);
                    id(ID).addEventListener("click", recoClipboard);
                }
                wordDiv = id("Word");

                document.body.addEventListener("keypress", keypress, false);

                if (!setRecognizerByName("Microsoft English (US) Handwriting Recognizer")) {
                    displayStatus("Failed to find English (US) recognizer");
                } else {
                    displayStatus("Verba volant, Scripta manet");
                }

                inkMode();
                renderPaper();
            }
        ).done(
            function () {
            },
            function (e) {
                displayError("inkInitialize " + e.toString());
            }
        );
    }

    // Tag the event handlers of the ToolBar so that they can be used in a declarative context.
    // For security reasons WinJS.UI.processAll and WinJS.Binding.processAll (and related) functions allow only
    // functions that are marked as being usable declaratively to be invoked through declarative processing.
    WinJS.UI.eventHandler(selectMode);
    WinJS.UI.eventHandler(eraseMode);
    WinJS.UI.eventHandler(clear);
    WinJS.UI.eventHandler(refresh);
    WinJS.UI.eventHandler(recognize);
    WinJS.UI.eventHandler(copySelected);
    WinJS.UI.eventHandler(paste);
    WinJS.UI.eventHandler(load);
    WinJS.UI.eventHandler(save);
    WinJS.Namespace.define("Ink", {
        selectMode: selectMode,
        eraseMode: eraseMode,
        clear: clear,
        refresh: refresh,
        recognize: recognize,
        copySelected: copySelected,
        paste: paste,
        load: load,
        save: save
    });

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            inkInitialize();
        }
    });
})();