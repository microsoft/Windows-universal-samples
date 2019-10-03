//// Copyright (c) Microsoft Corporation. All rights reserved

(function() {
    // Create a MediaSession and make a license request on 'message' events.
    // This is an event handler on the video object, so "this" is the video element.
    function onEncryptedRequest(e) {
        WinJS.log && WinJS.log("encyption request", "", "status");

        var video = this;
        var keySession = video.mediaKeys.createSession();
        keySession.addEventListener('message', function (event) {
            video.drmManager.downloadKey(video.drmManager.licenseUrl, event.message, function (data) {
                event.target.update(data);
                WinJS.log && WinJS.log("key message request completed", "", "status");
            });
        }, false);

        keySession.generateRequest(e.initDataType,  e.initData)
            .catch('error in keySession.generateRequest');
    }

    /**
     * desc@ formatSecureStopCDMData
     *   generate playready CDMData
     *   CDMData is in xml format:
     *   <PlayReadyCDMData type="SecureStop">
     *     <SecureStop version="1.0">
     *       <SessionID>B64 encoded session ID</SessionID>
     *       <CustomData>B64 encoded custom data</CustomData>
     *       <ServerCert>B64 encoded server cert</ServerCert>
     *     </SecureCert>
     * </PlayReadyCDMData>
     */
    function formatSecureStopCDMData(encodedSessionId, customData, encodedPublisherCert) {
        var rgbCustomData = stringToByteArray(customData, true);
        var uint16arrayCustomData = new Uint16Array(rgbCustomData);
        var customDataW = String.fromCharCode.apply(null, uint16arrayCustomData);
        var encodedCustomData = btoa(customDataW);

        var cmdData    = "<PlayReadyCDMData type=\"SecureStop\">" +
                         "<SecureStop version=\"1.0\" >" +
                         "<SessionID>" + encodedSessionId + "</SessionID>" +
                         "<CustomData>" + customData + "</CustomData>" +
                         "<ServerCert>" + encodedPublisherCert + "</ServerCert>" +
                         "</SecureStop></PlayReadyCDMData>";

        var rgbCDMData = stringToByteArray(cmdData, true);
        var int8ArrayCDMdata = new Uint8Array(rgbCDMData);

        return int8ArrayCDMdata;
    };

    var decode_map = {
        '&lt;': '<',
        '&gt;': '>'
    };

    function xmldecode(str) {
        return str.replace(/(&lt;|&gt;)/g,
            function (itm) {
                return decode_map[itm];
            });
    }

    function stringToByteArray(strIn, WideChar) {
        var rgbBytes = new Array();
        var strTmp = xmldecode(strIn);
        var cbBytes = strTmp.length;
        var i = 0;

        try {
            for (i = 0; i < cbBytes; i++) {
                var chr = strTmp.charCodeAt(i);
                rgbBytes.push(chr);
                if (WideChar != 'undefined' && WideChar == true) {
                    rgbBytes.push('\0'.charCodeAt(0));
                }
            }
        } catch (e) {
            WinJS.log("function(stringToByteArrayW): exception " + e, "", "error");
            return;
        }

        return rgbBytes;
    }

    // Find the name and value pairs in the key message XML and
    // the collection of XMLHttpRequest headers to include with the requst.
    function buildHeadersFromXml(xml) {
        var headerNames = xml.getElementsByTagName("name");
        var headerValues = xml.getElementsByTagName("value");

        if (headerNames.length !== headerValues.length) {
            throw 'Mismatched header <name>/<value> pair in key message';
        }
        var headers = {};
        for (var i = 0; i < headerNames.length; i++) {
            headers[headerNames[i].childNodes[0].nodeValue] = headerValues[i].childNodes[0].nodeValue;
        }
        return headers;
    }

    WinJS.Namespace.define("playready", {
        drmManager: WinJS.Class.define(
            // Constructor
            function drmManager(video, licenseUrl) {
                this.video = video;
                this.video.drmManager = this;

                // this licenseUrl will be used for all license service calls
                this.licenseUrl = licenseUrl;

                // Creates an instance of EME MediaKeys
                this.initializeMediaKeys();

                // Add an event listener to handle encrypted content
                this.video.addEventListener('encrypted', onEncryptedRequest, false);

                // Used for ms-prefixed capabilities such as secure stop messages
                this.msMediaKeys = new MSMediaKeys("com.microsoft.playready");
            },
            {
                // Instance properties
                publisherCert: '',

                // Creates an EME MediaKeySystemConfiguration
                keySystem: "com.microsoft.playready",
                keySystemConfig: [{
                    initDataTypes: ['keyids', 'cenc'],
                    audioCapabilities: [{ contentType: 'audio/mp4; codecs="mp4a"' }],
                    videoCapabilities: [{ contentType: 'video/mp4; codecs="avc1"' }]
                }],

                // Instance methods
                // Create MediaKeys from MediaKeySystemAccess based on the MediaKeySystemConfiguration info
                initializeMediaKeys: function initializeMediaKeys() {
                    var that = this;
                    this.mediaKeys = null;
                    navigator.requestMediaKeySystemAccess(this.keySystem, this.keySystemConfig).then(function (keySystemAccess) {
                        keySystemAccess.createMediaKeys().then(function (createdMediaKeys) {
                            that.mediaKeys = createdMediaKeys;
                            this.video.setMediaKeys(createdMediaKeys);
                        }).catch(function (e) {
                        });
                    });
                },

                // Use XMLHttpRequest to post the license request to PlayReady.
                // Process the response with the callback
                downloadKey: function downloadKey(url, keyMessage, callback) {
                    var challenge;
                    var keyMessageXml = new DOMParser().parseFromString(String.fromCharCode.apply(null, new Uint16Array(keyMessage)), 'application/xml');
                    var challengeElement = keyMessageXml.getElementsByTagName('Challenge')[0];
                    if (challengeElement) {
                        challenge = atob(challengeElement.childNodes[0].nodeValue);
                    } else {
                        throw 'Cannot find <Challenge> in key message';
                    }

                    var challenge;
                    WinJS.xhr({
                        type: "POST",
                        url: url,
                        responseType: "arraybuffer",
                        headers: buildHeadersFromXml(keyMessageXml),
                        data: challenge
                    }).then(function complete(xhr) {
                        callback(xhr.response);
                    }, function error(xhr) {
                        var msg = 'XHR failed (' + url + '). Status: ' + xhr.status + ' (' + xhr.statusText + ')';
                        WinJS.log && WinJS.log(msg, "", "error");
                    });
                },

                // Use XMLHttpRequest to acquire a Publisher Cert used to send
                // Secure Stop messages
                getPublisherCert: function getPublisherCert(url, publisherID, callback) {
                    var that = this;
                    var meteringCertChallenge = '<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"><soap:Body><GetMeteringCertificate xmlns="http://schemas.microsoft.com/DRM/2007/03/protocols"><challenge><Challenge><MeterCert><Version>1</Version><MID>' + publisherID + '</MID></MeterCert></Challenge></challenge></GetMeteringCertificate></soap:Body></soap:Envelope>';
                    var certMessageXml = new DOMParser().parseFromString(meteringCertChallenge, "application/xml");
                    var challenge = new XMLSerializer().serializeToString(certMessageXml);

                    WinJS.xhr({
                        type: "POST",
                        url: url,
                        headers: {
                            "SOAPAction": "http://schemas.microsoft.com/DRM/2007/03/protocols/GetMeteringCertificate",
                            "Content-Type": "text/xml; charset=utf-8"
                        },
                        data: challenge
                    }).then(function complete(xhr) {
                        var responseXml = new DOMParser().parseFromString(xhr.response.toString(), "application/xml");
                        that.publisherCert = responseXml.getElementsByTagName("MeterCert")[0].childNodes[0].nodeValue;
                        callback(xhr.response);
                    }, function error(xhr) {
                        var msg = 'XHR failed (' + url + '). Status: ' + xhr.status + ' (' + xhr.statusText + ')';
                        WinJS.log && WinJS.log(msg, "", "error");
                    });
                },

                createSecureStopCDMSession: function createSecureStopCDMSession() {
                    try {
                        var targetMediaCodec = "video/mp4";
                        var customData = "";

                        // you can use "*" (wildcard) as the session ID to include all secure stop sessions
                        var sessionId = "KgA="; //"*"

                        var int8ArrayCDMdata = formatSecureStopCDMData(sessionId, customData, this.publisherCert);
                        var emptyArray = new Uint8Array();

                        this.secureStopSession = this.msMediaKeys.createSession(targetMediaCodec, emptyArray, int8ArrayCDMdata);

                        this.addPlayreadyKeyEventHandlers(this.secureStopSession);
                    } catch (e) {
                        // TODO: Handle exception
                    }
                },

                addPlayreadyKeyEventHandlers: function addPlayreadyKeyEventHandlers(session) {
                    var that = this;

                    session.addEventListener('mskeymessage', function (e) {
                        var keyMessage = String.fromCharCode.apply(null, new Uint16Array(e.message.buffer));
                        var keyMessageXML = new DOMParser().parseFromString(keyMessage, "application/xml");
                        var challenge = atob(keyMessageXML.getElementsByTagName("Challenge")[0].childNodes[0].nodeValue);

                        WinJS.xhr({
                            type: "POST",
                            url: that.url,
                            responseType: "arraybuffer",
                            headers: buildHeadersFromXml(keyMessageXML),
                            data: challenge
                        }).then(function complete() {
                            WinJS.log && WinJS.log("Got License Acquisition response", "", "status");
                        });
                    });

                    session.addEventListener("mskeyadded", function () {
                        WinJS.log("Secure stop: Key successfully added with session ID=" + session.sessionId, "", "status");
                        that.completeCallback && that.completeCallback();
                    });

                    session.addEventListener("mskeyerror", function () {
                        if (session.error.systemCode == -2147024637) { // -2147024637 == DRM_E_NOMORE
                            WinJS.log("No secure stop session to process.", "", "status");
                        } else {
                            var msg = "SecureStop: Unexpected 'keyerror' event from key session. Code: " + session.error.code + ", systemCode: " + session.error.systemCode;
                            WinJS.log(msg, "", "error");
                        }
                    });
                }
            })
    });
})();
