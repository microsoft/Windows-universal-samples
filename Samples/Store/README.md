<!---
  category: AppSettings TrialsAdsAndInAppPurchases
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=627610
--->

# Store sample

Shows how to perform in-app feature or product purchases operations
and use the licensing API provided by the Windows Store
to determine the license status of an app or a feature that is enabled by an in-app purchase.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The Windows Store supports the following types of in-app purchases:

-   Purchasing the app itself.
-   Purchasing a durable in-app product, meaning that after the purchase, the user owns the product until it (optionally) expires.
    For example, a new game level is typically a durable in-app product.
-   Purchasing a consumable in-app product, meaning that the user can purchase it multiple times.
    For example, a game power-up is typically a consumable in-app product.
    The act of confirming that the user received the consumable product is known as "fulfillment".

The sample app demonstrates the following tasks.

-   Check the current license status of an app.
-   Check the expiration date of an app trial period.
-   Check if an in-app product has been purchased through an in-app purchase.
-   Perform an in-app purchase to buy the app.
-   Perform an in-app purchase to buy an in-app product.
-   Confirm the fulfillment of an in-app consumable product.
-   Perform an in-app purchase transaction using the Windows Store commerce platform.
-   Launch the Windows Store page for the app, so users can purchase the app or write a review.
-   Generate a business to business (B2B) token that can be used to query or add to a user's collection.

### Setup Instructions

Before running this sample, you must associate it with an app in the Store.

1. [Register as an app developer](https://developer.microsoft.com/store/register).
2. Follow the instructions in
[Testing apps that use the Windows.Services.Store namespace](https://msdn.microsoft.com/windows/uwp/monetize/in-app-purchases-and-trials#testing-apps-that-use-the-windows-services-store-namespace).

### Additional Setup Instructions for Business to Business Scenario

These steps are necessary only if you want to run the code in the Business to Business Scenario, which requires application and Azure configuration.

1. You must have a real application available for download from the Windows Store.  This can be a private application.
2. Download a copy of your application from the store to get a valid license.
3. Associate this test application with this application in the store.
4. Deploy this application to your machine and overwrite the existing application.
5. Setup your application following the instructions under the **Adding an Application** section at [Integrating Applications with Azure Active Directory](https://azure.microsoft.com/documentation/articles/active-directory-integrating-applications/).
6. Replace the provided value of "tenantId" with your actual tenant ID for your Azure account.
7. Replace the provided value of "clientId" with the actual value of your native application created in Azure.
8. Replace the provided value of "clientSecret" with the actual value for your Azure account.
9. Replace the provided value of "appIdUri" with the actual value of your Azure application.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

[Coloring Book app sample](https://github.com/Microsoft/Windows-appsample-coloringbook)  

### Samples

[Advertising](/Samples/Advertising)  

### Reference

[Windows.Services.Store namespace](https://msdn.microsoft.com/library/windows/apps/windows.services.store.aspx)  
[Monetization, customer engagement, and Store services](https://msdn.microsoft.com/windows/uwp/monetize/)  

## System requirements

**Client:** Windows 10 version 1607

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10 version 1607

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample on a Windows 10 Desktop

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

### Deploying and running the sample on a Windows 10 Phone

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
