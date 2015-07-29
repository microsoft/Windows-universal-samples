This sample demonstrates Enterprise Data Protection APIs: File and Buffer protection APIs, clipboard APIs, DPL APIs and networking APIs. All APIs except File and Buffer protection APIs require Enterprise data policy to be set for a specific identity. 

If you have EDP policy set for a specific identity, you should expect the IsIdentityManaged API to return true. If the API returns false, all APIs except the File and Buffer APIs will not work as expected. Each API has a detailed scenario description that describes the purpose and behavior of the API.

You need to add a restricted capability called EnterpriseDataPolicy into your application manifest file to mark your app enlightened. 

Related Topics
--------------

[Enterprise Data Protection Overview](https://msdn.microsoft.com/en-us/library/Dn985838(v=VS.85).aspx)

System requirements
-------------------

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

