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

// Common helper methods

function getError(e) {
    "use strict";

    var webSocketError = Windows.Networking.Sockets.WebSocketError.getStatus(e.number);
    switch (webSocketError) {
        case Windows.Web.WebErrorStatus.unknown: return e;
        case Windows.Web.WebErrorStatus.certificateCommonNameIsIncorrect: return "CertificateCommonNameIsIncorrect";
        case Windows.Web.WebErrorStatus.certificateExpired: return "CertificateExpired";
        case Windows.Web.WebErrorStatus.certificateContainsErrors: return "CertificateContainsErrors";
        case Windows.Web.WebErrorStatus.certificateRevoked: return "CertificateRevoked";
        case Windows.Web.WebErrorStatus.certificateIsInvalid: return "CertificateIsInvalid";
        case Windows.Web.WebErrorStatus.serverUnreachable: return "ServerUnreachable";
        case Windows.Web.WebErrorStatus.timeout: return "Timeout";
        case Windows.Web.WebErrorStatus.errorHttpInvalidServerResponse: return "ErrorHttpInvalidServerResponse";
        case Windows.Web.WebErrorStatus.connectionAborted: return "ConnectionAborted";
        case Windows.Web.WebErrorStatus.connectionReset: return "ConnectionReset";
        case Windows.Web.WebErrorStatus.disconnected: return "Disconnected";
        case Windows.Web.WebErrorStatus.httpToHttpsOnRedirection: return "HttpToHttpsOnRedirection";
        case Windows.Web.WebErrorStatus.httpsToHttpOnRedirection: return "HttpsToHttpOnRedirection";
        case Windows.Web.WebErrorStatus.cannotConnect: return "CannotConnect";
        case Windows.Web.WebErrorStatus.hostNameNotResolved: return "HostNameNotResolved";
        case Windows.Web.WebErrorStatus.operationCanceled: return "OperationCanceled";
        case Windows.Web.WebErrorStatus.redirectFailed: return "RedirectFailed";
        case Windows.Web.WebErrorStatus.unexpectedStatusCode: return "UnexpectedStatusCode";
        case Windows.Web.WebErrorStatus.unexpectedRedirection: return "UnexpectedRedirection";
        case Windows.Web.WebErrorStatus.unexpectedClientError: return "UnexpectedClientError";
        case Windows.Web.WebErrorStatus.unexpectedServerError: return "UnexpectedServerError";
        case Windows.Web.WebErrorStatus.multipleChoices: return "MultipleChoices (300)";
        case Windows.Web.WebErrorStatus.movedPermanently: return "MovedPermanently (301)";
        case Windows.Web.WebErrorStatus.found: return "Found (302)";
        case Windows.Web.WebErrorStatus.seeOther: return "SeeOther (303)";
        case Windows.Web.WebErrorStatus.notModified: return "NotModified (304)";
        case Windows.Web.WebErrorStatus.useProxy: return "UseProxy (305)";
        case Windows.Web.WebErrorStatus.temporaryRedirect: return "TemporaryRedirect (307)";
        case Windows.Web.WebErrorStatus.badRequest: return "BadRequest (400)";
        case Windows.Web.WebErrorStatus.unauthorized: return "Unauthorized (401)";
        case Windows.Web.WebErrorStatus.paymentRequired: return "PaymentRequired (402)";
        case Windows.Web.WebErrorStatus.forbidden: return "Forbidden (403)";
        case Windows.Web.WebErrorStatus.notFound: return "NotFound (404)";
        case Windows.Web.WebErrorStatus.methodNotAllowed: return "MethodNotAllowed (405)";
        case Windows.Web.WebErrorStatus.notAcceptable: return "NotAcceptable (406)";
        case Windows.Web.WebErrorStatus.proxyAuthenticationRequired: return "ProxyAuthenticationRequired (407)";
        case Windows.Web.WebErrorStatus.requestTimeout: return "RequestTimeout (408)";
        case Windows.Web.WebErrorStatus.conflict: return "Conflict (409)";
        case Windows.Web.WebErrorStatus.gone: return "Gone (410)";
        case Windows.Web.WebErrorStatus.lengthRequired: return "LengthRequired (411)";
        case Windows.Web.WebErrorStatus.preconditionFailed: return "PreconditionFailed (412)";
        case Windows.Web.WebErrorStatus.requestEntityTooLarge: return "RequestEntityTooLarge (413)";
        case Windows.Web.WebErrorStatus.requestUriTooLong: return "RequestUriTooLong (414)";
        case Windows.Web.WebErrorStatus.unsupportedMediaType: return "UnsupportedMediaType (415)";
        case Windows.Web.WebErrorStatus.requestedRangeNotSatisfiable: return "RequestedRangeNotSatisfiable (416)";
        case Windows.Web.WebErrorStatus.expectationFailed: return "ExpectationFailed (417)";
        case Windows.Web.WebErrorStatus.internalServerError: return "InternalServerError (500)";
        case Windows.Web.WebErrorStatus.notImplemented: return "NotImplemented (501)";
        case Windows.Web.WebErrorStatus.badGateway: return "BadGateway (502)";
        case Windows.Web.WebErrorStatus.serviceUnavailable: return "ServiceUnavailable (503)";
        case Windows.Web.WebErrorStatus.gatewayTimeout: return "GatewayTimeout (504)";
        case Windows.Web.WebErrorStatus.httpVersionNotSupported: return "HttpVersionNotSupported (505)";
        default: return e;
    }
};

function validateAndCreateUri(uriString) {
    "use strict";

    var webSocketUri;
    try {
        webSocketUri = new Windows.Foundation.Uri(uriString);
    } catch (error) {
        throw "Error: Invalid URI";
    }

    if (webSocketUri.fragment !== "") {
        throw "Error: URI fragments not supported in WebSocket URIs.";
    }

    // Uri.schemeName returns the canonicalized scheme name so we can use case-sensitive, ordinal string
    // comparison.
    var scheme = webSocketUri.schemeName;
    if ((scheme !== "ws") && (scheme !== "wss")) {
        throw "Error: WebSockets only support ws:// and wss:// schemes.";
    }

    return webSocketUri;
}
