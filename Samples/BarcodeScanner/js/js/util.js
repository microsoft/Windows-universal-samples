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

(function () {

    var BarcodeSymbologies = Windows.Devices.PointOfService.BarcodeSymbologies;

    getDataLabelString = function (data, scanDataType) {

        var result = null;

        // Only certain data types contain encoded text.
        //   To keep this simple, we'll just decode a few of them.
        if (data === null) {
            result = "No data";
        } else {
            switch (scanDataType) {
                case BarcodeSymbologies.upca:
                case BarcodeSymbologies.upcaAdd2:
                case BarcodeSymbologies.upcaAdd5:
                case BarcodeSymbologies.upce:
                case BarcodeSymbologies.upceAdd2:
                case BarcodeSymbologies.upceAdd5:
                case BarcodeSymbologies.ean8:
                case BarcodeSymbologies.tfStd:
                case BarcodeSymbologies.ocrA:
                case BarcodeSymbologies.ocrB:
                    // The UPC, EAN8, and 2 of 5 families encode the digits 0..9
                    // which are then sent to the app in a UTF8 string (like "01234").
                    // This is not an exhaustive list of symbologies that can be converted to a string

                    var reader = Windows.Storage.Streams.DataReader.fromBuffer(data);
                    result = reader.readString(data.length);
                    break;

                default:
                    // Some other symbologies (typically 2-D symbologies) contain binary data that
                    //  should not be converted to text.
                    result = "Decoded data unavailable. Raw label data: " + getDataString(data);
                    break;
            }
        }

        return result;
    };


    getDataString = function (data) {
        var result = "";

        if (data === null) {
            result = "No data";
        }
        else {
            // Just to show that we have the raw data, we'll print the value of the bytes.
            // Arbitrarily limit the number of bytes printed to 20 so the UI isn't overloaded.
            var MAX_BYTES_TO_PRINT = 20;
            var bytesToPrint = (data.length < MAX_BYTES_TO_PRINT) ? data.length : MAX_BYTES_TO_PRINT;

            var reader = Windows.Storage.Streams.DataReader.fromBuffer(data);

            for (var byteIndex = 0; byteIndex < bytesToPrint; ++byteIndex) {
                result += reader.readByte().toString(16) + " ";
            }

            if (bytesToPrint < data.length) {
                result += "...";
            }
        }

        return result;
    }

})();
