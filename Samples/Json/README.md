<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620556
--->

# JSON sample

Shows how to encode and decode JavaScript Object Notation (JSON) objects, arrays, strings, numbers and booleans using classes in the 
[Windows.Data.Json](http://msdn.microsoft.com/library/windows/apps/br240639) namespace. This namespace is intended for C\# and 
C++ programming languages. JavaScript developers should use the [JSON.Parse](http://go.microsoft.com/fwlink/p/?linkid=398621) method 
to parse JSON. For more information, see the [JSON Object (JavaScript)](http://go.microsoft.com/fwlink/p/?linkid=398620), an intrinsic 
object that provides functions to convert JavaScript values to and from the JSON format.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates the following features:

-   Serialize and deserialize JSON objects using the [JsonObject](http://msdn.microsoft.com/library/windows/apps/br225267) class.
-   Serialize and deserialize JSON arrays using the [JsonArray](http://msdn.microsoft.com/library/windows/apps/br225234) class.
-   Serialize and deserialize strings using the [JsonValue](http://msdn.microsoft.com/library/windows/apps/br240622) class.
-   Serialize and deserialize numbers using the [JsonValue](http://msdn.microsoft.com/library/windows/apps/br240622) class.
-   Serialize and deserialize booleans using the [JsonValue](http://msdn.microsoft.com/library/windows/apps/br240622) class.
-   Serialize and deserialize null using the [JsonValue](http://msdn.microsoft.com/library/windows/apps/br240622) class.

The following features can be used to retrieve text that contains JSON from an HTTP server or send text that contains JSON to an HTTP server.

-   [Windows.Web.Http.HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639) - Supports Universal Windows Platform (UWP) apps written in JavaScript, C\#, Visual Basic, or C++.
-   [XML HTTP Extended Request (IXMLHttpRequest2)](http://msdn.microsoft.com/library/windows/apps/hh831163) - Supports UWP apps written in C++.
-   [C++ REST SDK](http://msdn.microsoft.com/library/jj988008.aspx) - Supports UWP apps written in C++.

**Note** Use of this sample does not require Internet or intranet access so no network capabilities need to be set in the *Package.appmanifest* file.


## Related topics

### Other

[Connecting to web services (Universal Windows Platform app using C++, C\#, or Visual Basic)](http://msdn.microsoft.com/library/windows/apps/hh761504)  
[Using JavaScript Object Notation (JSON) (Universal Windows Platform app using C++, C\#, or Visual Basic)](http://msdn.microsoft.com/library/windows/apps/hh770289)  

### Reference

[C++ REST SDK](http://msdn.microsoft.com/library/jj988008.aspx)  
[JsonArray](http://msdn.microsoft.com/library/windows/apps/br225234)  
[JsonObject](http://msdn.microsoft.com/library/windows/apps/br225267)  
[JsonValue](http://msdn.microsoft.com/library/windows/apps/br240622)  
[Windows.Data.Json](http://msdn.microsoft.com/library/windows/apps/br240639)  
[Windows.Web.Http.HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639)  
[XML HTTP Extended Request (IXMLHttpRequest2)](http://msdn.microsoft.com/library/windows/apps/hh831163)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

