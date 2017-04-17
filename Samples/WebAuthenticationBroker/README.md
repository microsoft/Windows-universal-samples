<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620622
--->

# WebAuthenticationBroker sample

Shows how to use the WebAuthenticationBroker class to connect to OAuth providers such as Facebook, Flickr, Google, and Twitter.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows you how to use the OAuth protocol for authentication and authorization. You can choose one of three scenarios: 

- Facebook [C++, C#, JS]
- Twitter [C#, JS]
- Flickr [C#, JS]
- Google [C++, C#, JS]

## Related Topics

[Quickstart: Connecting using XML HTTP Request](http://msdn.microsoft.com/library/windows/apps/hh770550)  
[Setting up single sign-on using the web authentication broker](http://msdn.microsoft.com/library/windows/apps/hh465283)  

[WebAccount](http://msdn.microsoft.com/library/windows/apps/dn279122)  
[WebAccountCommand](http://msdn.microsoft.com/library/windows/apps/dn298413)  
[WebAccountProvider](http://msdn.microsoft.com/library/windows/apps/dn279123)  
[WebAuthenticationBroker](http://msdn.microsoft.com/library/windows/apps/br227025)  
[Windows.Security.Authentication.Web](http://msdn.microsoft.com/library/windows/apps/br227044)  

## Obtaining Client Ids For Use with OAuth APIs

To configure Google OAuth connections (Google Apps and Google) you will need to register Auth0 with Google on the API Console.
- Log in to [API Console](https://console.developers.google.com/)
- Complete information about your instance of Auth0. 
- Enable the Google+ API. 
- Set up Consent Screen. 
- Get your ClientId and ClientSecret. 
- Enable Admin SDK Service.

To configure Facebook OAuth connections you will have to register as a Facebook Developer
- [Becoming a Facebook Developer](https://developers.facebook.com/apps)

To configure Twitter OAuth connections you will have to register an application with Twitter
- [xAuth for OAuth access to Twitter APIs](https://dev.twitter.com/oauth/xauth)

To configure Flickr OAuth connections you will have to register an application with Flickr
- [Flickr API Keys Documentation](https://www.flickr.com/services/api/misc.api_keys.html)

## Operating system requirements

Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

## Deploying the sample

1.  Select **Build** \> **Deploy Solution**.

## Deploying and running the sample

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.
