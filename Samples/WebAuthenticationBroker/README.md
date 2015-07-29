This sample shows how you can use the WebAuthenticationBroker class to connect to OAuth providers such as Facebook, Flickr, Google, and Twitter.

**Note** This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

Shows using the OAuth protocol for authentication and authorization. You can choose one of three scenarios: 
- Facebook [C++, C#, JS]
- Twitter [C#, JS]
- Flickr [C#, JS]
- Google [C++, C#, JS]

Related Topics
--------------
[Web Authentication Broker](http://msdn.microsoft.com/library/windows/apps/)
[Quickstart: Connecting using XML HTTP Request](http://msdn.microsoft.com/library/windows/apps/hh770550)
[Setting up single sign-on using the web authentication broker](http://msdn.microsoft.com/library/windows/apps/hh465283)

[WebAccount](http://msdn.microsoft.com/library/windows/apps/dn279122)
[WebAccountCommand](http://msdn.microsoft.com/library/windows/apps/dn298413)
[WebAccountProvider](http://msdn.microsoft.com/library/windows/apps/dn279123)
[WebAuthenticationBroker](http://msdn.microsoft.com/library/windows/apps/br227025)
[Windows.Security.Authentication.Web](http://msdn.microsoft.com/library/windows/apps/br227044)

Obtaining Client Ids For Use with OAuth APIs
--------------------------------------------
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

Operating system requirements
-----------------------------

Windows 10

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++ or C\#. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.
