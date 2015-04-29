Json Sample
===========

This sample shows you how to encode and decode JavaScript Object Notation (JSON) objects, arrays, strings, numbers and booleans using classes in the [**Windows.Data.Json**](http://msdn.microsoft.com/library/windows/apps/br240639) namespace. This namespace is intended for C\# and C++ programming languages. JavaScript developers should use the [JSON.Parse](http://go.microsoft.com/fwlink/p/?linkid=398621) method to parse JSON. For more information, see the [JSON Object (JavaScript)](http://go.microsoft.com/fwlink/p/?linkid=398620), an intrinsic object that provides functions to convert JavaScript values to and from the JSON format.

This sample demonstrates the following features:

-   Serialize and deserialize JSON objects using the [**JsonObject**](http://msdn.microsoft.com/library/windows/apps/br225267) class.
-   Serialize and deserialize JSON arrays using the [**JsonArray**](http://msdn.microsoft.com/library/windows/apps/br225234) class.
-   Serialize and deserialize strings using the [**JsonValue**](http://msdn.microsoft.com/library/windows/apps/br240622) class.
-   Serialize and deserialize numbers using the [**JsonValue**](http://msdn.microsoft.com/library/windows/apps/br240622) class.
-   Serialize and deserialize booleans using the [**JsonValue**](http://msdn.microsoft.com/library/windows/apps/br240622) class.
-   Serialize and deserialize null using the [**JsonValue**](http://msdn.microsoft.com/library/windows/apps/br240622) class.

The following features can be used to retrieve text that contains JSON from an HTTP server or send text that contains JSON to an HTTP server.

-   [**Windows.Web.Http.HttpClient**](http://msdn.microsoft.com/library/windows/apps/dn298639) - Supports apps written in JavaScript, C\#, Visual Basic, or C++.
-   [XML HTTP Extended Request (IXMLHttpRequest2)](http://msdn.microsoft.com/library/windows/apps/hh831163) - Supports apps written in C++.
-   [C++ REST SDK](http://msdn.microsoft.com/en-us/library/jj988008(v=vs.120).aspx) - Supports apps written in C++.

**Note** Use of this sample does not require Internet or intranet access so no network capabilities need to be set in the *Package.appmanifest* file.


Related topics
--------------

**Other**

[Connecting to web services (Windows Runtime app using C++, C\#, or Visual Basic)](http://msdn.microsoft.com/library/windows/apps/hh761504)

[Using JavaScript Object Notation (JSON) (Windows Runtime app using C++, C\#, or Visual Basic)](http://msdn.microsoft.com/library/windows/apps/hh770289)

**Reference**

[C++ REST SDK](http://msdn.microsoft.com/en-us/library/jj988008(v=vs.120).aspx)

[**JsonArray**](http://msdn.microsoft.com/library/windows/apps/br225234)

[**JsonObject**](http://msdn.microsoft.com/library/windows/apps/br225267)

[**JsonValue**](http://msdn.microsoft.com/library/windows/apps/br240622)

[**Windows.Data.Json**](http://msdn.microsoft.com/library/windows/apps/br240639)

[**Windows.Web.Http.HttpClient**](http://msdn.microsoft.com/library/windows/apps/dn298639)

[XML HTTP Extended Request (IXMLHttpRequest2)](http://msdn.microsoft.com/library/windows/apps/hh831163)



Operating system requirements
-----------------------------

Client

Windows 10 Technical Preview

Server

Windows 10 Technical Preview

Phone

Windows 10 Technical Preview

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Go to the directory named for the sample, and double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or use **Build** \> **Build Solution**

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

-   To deploy the sample: Use **Build** \> **Deploy Solution**


**Deploying and running the sample**

-   To deploy and run the sample: Press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

