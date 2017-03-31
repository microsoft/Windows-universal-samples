<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620542
--->

# RSS reader end-to-end (JavaScript) sample 

Shows a basic end-to-end implementation of a news feed reader. Uses a [ListView](http://msdn.microsoft.com/library/windows/apps/br242878) 
to organize and display articles from various subscriptions specified in a JSON-formatted data file. The data is obtained over a network connection or from a local cache.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers these news reader features and Windows Store app APIs.

This app includes these news reader features and Windows Store app APIs.

-   A pannable [ListView](http://msdn.microsoft.com/library/windows/apps/br242878) page showing the subscribed news feeds and up to 15 articles per subscription.

    Sample: Look at the news.css, news.html, and news.js files in the \\pages\\news\\ folder.

-   A section page showing all available articles in the selected subscription.

    Sample: See feed.css, feed.html, and feed.js files in the \\pages\\feed\\ folder.

-   A detail page showing the content of the article selected from the [ListView](http://msdn.microsoft.com/library/windows/apps/br242878) or section page.

    Sample: Examine article.css, article.html, and article.js files in the \\pages\\article\\ folder.

-   A subscription page showing all available news feeds. Users can subscribe to a news feed by selecting it.

    Sample:

    -   Look at the subscriptions.css, subscriptions.html, and subscriptions.js files in the \\pages\\subscriptions\\ folder and the feedCollection.json file in the Project folder.
    -   For default subscriptions, see `var defaultSubscriptions = ["Engadget", "Windows App Builder Blog"];` in io.js in the \\js folder.
    -   Maximum subscriptions and articles are specified by `// Constants defining limits of the ListView.     WinJS.Namespace.define("ListViewLimits", {         subLimit: 10,         maxItems: 15     });` in default.js in the \\js folder.
-   Data structure representing the available newsfeeds, subscribed newsfeeds, and news articles.

    Sample: See data.js in the \\js folder.

-   Loading, formatting, and displaying of news reader data.

    Sample: Review io.js and render.js files in the \\js folder.

Here are some general Windows Store app features demonstrated by this app.

-   Splash screen on start up.

    Sample: Check `SplashScreen` in the package.appxmanifest file in the Project folder and splashscreen.png in the \\images folder.

-   Single page architecture.

    Sample: Inspect the `<div id="contenthost" data-win-control="Application.PageControlNavigator" data-win-options="{home: '/pages/news/news.html'}"></div>` element in default.html, and navigator.js in the \\js folder.

-   App bar with **Refresh** command and Nav bar with a link to a **Subscriptions** page.

    Sample: Examine the `<div id="appbar" data-win-control="WinJS.UI.AppBar">` node in default.html.

-   Semantic Zoom to view the subscribed news feeds or individual articles for the selected news feed.

    Sample: View the `<div id="articlesListViewArea" data-win-control="WinJS.UI.SemanticZoom">` and `<div id="articlesListView-out" aria-label="List of feeds" data-win-control="WinJS.UI.ListView"                       data-win-options="{ selectionMode: 'none', tapBehavior: 'invokeOnly', swipeBehavior: 'none' }">` nodes in news.html and the initializeLayout function in news.js, both of which are in the \\pages\\news folder.

-   Adaptive layouts to rearrange content when the app is resized.

    Sample: Review article.css in the \\pages\\article folder and feed.css in the \\pages\\feed folder.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

**Samples**

[RSS reader (XAML MVVM) end-to-end sample](https://github.com/Microsoft/Windows-appsample-rssreader)  
[Other samples](http://microsoft.github.io/windows/)  
[Older end-to-end sample apps](http://msdn.microsoft.com/library/windows/apps/dn263104)  

**Conceptual**

[Adding ListView controls](http://msdn.microsoft.com/library/windows/apps/hh465382)  
[Navigation design for Windows Store apps](http://msdn.microsoft.com/library/windows/apps/hh761500)  
[Connecting to networks and web services (JavaScript)](http://msdn.microsoft.com/library/windows/apps/br211370)  
[Developing connected applications](http://msdn.microsoft.com/library/windows/apps/hh465399)  

**Tasks**

[Quickstart: Adding a splash screen](http://msdn.microsoft.com/library/windows/apps/hh465346)  
[Quickstart: Using single-page navigation](http://msdn.microsoft.com/library/windows/apps/hh452768)  
[Quickstart: adding an app bar with commands](http://msdn.microsoft.com/library/windows/apps/hh465309)  
[Quickstart: adding a SemanticZoom](http://msdn.microsoft.com/library/windows/apps/hh465492)  
[Quickstart: Defining app layouts](http://msdn.microsoft.com/library/windows/apps/jj150600)  

**Reference (feeds)**

[JSON Object](http://go.microsoft.com/fwlink/p/?linkid=308896)  
[Uri](http://msdn.microsoft.com/library/windows/apps/br225998)  
[SyndicationClient](http://msdn.microsoft.com/library/windows/apps/br243456)  
[NetworkInformation](http://msdn.microsoft.com/library/windows/apps/br207293)  
[ReadTextAsync](http://msdn.microsoft.com/library/windows/apps/hh701482)  
[readText](http://msdn.microsoft.com/library/windows/apps/hh700824)  

**Reference (general)**

[WinJS.Application Namespace](http://msdn.microsoft.com/library/windows/apps/br229774)  
[WinJS.Class Namespace](http://msdn.microsoft.com/library/windows/apps/br229776)  
[WinJS.Utilities Namespace](http://msdn.microsoft.com/library/windows/apps/br229783)  
[WinJS.Namespace Namespace](http://msdn.microsoft.com/library/windows/apps/br212652)  
[WinJS.Navigation Namespace](http://msdn.microsoft.com/library/windows/apps/br229778)  
[Windows.ApplicationModel.Activation Namespace](http://msdn.microsoft.com/library/windows/apps/br224766)  
[WinJS.Binding Namespace](http://msdn.microsoft.com/library/windows/apps/br229775)  
[WinJS.Promise](http://msdn.microsoft.com/library/windows/apps/br211867)  
[ListView](http://msdn.microsoft.com/library/windows/apps/br211837)  
[Template](http://msdn.microsoft.com/library/windows/apps/br229723)  
[SemanticZoom](http://msdn.microsoft.com/library/windows/apps/br229690)  

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

1.  Open the sample's project in Visual Studio 2015.
2.  To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

App highlights:

-   When the app starts for the first time, the main page displays two default feed subscriptions with articles.
-   Use Semantic Zoom to switch between subscription views. Zoom out by pinching (click the Minus button in the lower right corner with the mouse, Ctrl+Minus key with a keyboard) and zoom in by stretching (click a subscription with the mouse, Ctrl+Plus key with a keyboard).
-   Swipe from the top edge of the display (or right-click with a mouse, Windows Logo Key+Z or menu key with a keyboard) to display the Nav bar and the App bar.
-   Use the **Refresh** command on the App bar to sync the subscriptions.
-   Use the **Subscriptions** page link on the Nav bar to view the **Subscriptions** page. Select feeds to subscribe to and display on the main page. Tap **Done** when finished.
-   Use Semantic Zoom to switch between feed views on the **Subscriptions** page. Zoom out by pinching (click the Minus button in the lower right corner with the mouse, Ctrl+Minus key with a keyboard) and zoom in by stretching (click a feed with the mouse, Ctrl+Plus key with a keyboard).

