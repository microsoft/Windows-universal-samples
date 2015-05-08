This sample demonstrates Enterprise Data Protection APIs: File and Buffer protection APIs, clipboard APIs,
DPL APIs and networking APIs. All APIs except File and Buffer protection APIs require Enterprise data policy to be set for a specific identity. 

If you have EDP policy set for a specific identity, you should expect the IsIdentityManaged API to return true. If the API returns false, all APIs except the
File and Buffer APIs will not work as expected. Each API has a detailed scenario description that describes the purpose and behavior of the API.

You need to add a restricted capability called EnterpriseDataPolicy into your application manifest file to mark your app enlightened.  



System requirements
-----------------------------
Windows Threshold



