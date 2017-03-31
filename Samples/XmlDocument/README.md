<!---
  category: Data
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620639
--->

# XML DOM sample

Shows common XML API use scenarios for the Windows.Data.Xml.Dom and Windows.Data.Xml.Xsl namespaces in the Windows Runtime.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers:

- **Scenario 1:** Loading an RSS template XML file asynchronously and adding a CData section. The asynchronous loading is accomplished by using the 
**XmlDocument.loadFromFileAsync** method from the **Windows.Data.Xml.Dom** namespace. The **XmlDocument.CreateCDataSection** method is used to add the 
CDATA section to the RSS file.

- **Scenario 2:** Traversing a Document Object Model (DOM) tree to update the data and save it to a file. When the user presses the **"Mark Hot Products"** 
button, an XPath query is used to select products from an XML file, which have an attribute named "Sell10day" that exceeds the attribute named "InStore".  
The "Sell10Day" value represents the quantity sold in the last ten days. These products are marked "hot" by setting the "hot" attribute for the product to 1. 
When the user presses the **"Save"** button, the **XmlDocument.saveToFileAsync** method is used to save the XML file containing the products list.

- **Scenario 3:** Using the security settings prohibitDTD and resolveExternals when loading an XML file. prohibitDTD will prevent DTD use in a XML file. 
A large number of DTD entity references can cause an application to become unresponsive when loading and parsing an XML file. resolveExternals determines 
whether external references can be resolved during parsing. For this scenario, a user chooses a combination of these two security settings, which are 
properties on the **XmlLoadSettings** class used with method **XmlDocument.LoadFromFileAsync** and **XmlDocument.LoadXmlFromBuffer**. The user presses 
the **"Load XML From File"** button to view the resulting XML by **LoadFromFileAsync** or **"Load XML From Buffer"** button to view the resulting XML by **LoadXmlFromBuffer**.

- **Scenario 4:** Using an XPath with the XML API for the Windows Runtime to select and filter data in a DOM tree. When a user presses the **"Show Anniversary Gift"** 
button, the scenario constructs an array of XPath queries used by the **XmlDocument.SelectNodes** method against an XML file to determine an anniversary gift for 
each employee based on the year the employee started the job. Employees with between 1 and 4 years on the job receive a gift card. Employees with between 5 and 9 years 
on the job receive an XBOX. Employees with 10 or more years on the job receive a Windows Phone.

- **Scenario 5:** Using eXtensible Stylesheet Language Transformations (XSLT) with the XML APIs to transform an XML file into an HTML webpage. The source XML and XSL 
are loaded during initialization of the scenario. The XSL transform is applied when the user presses the **"TransformToString"** and **"TransformToDocument"** button. 
The Source XML and Source XSL can both be modified during runtime to see different results when applying an XSL transform. The transform is applied using the 
**XsltProcessor** class from the **Windows.Data.Xml.Xsl** namespace.

## Related topics

[Windows.Data.Xml.Dom.XmlDocument](https://msdn.microsoft.com/library/windows/apps/br206173)  
[Windows.Data.Xml.Xsl.XsltProcessor](https://msdn.microsoft.com/library/windows/apps/windows.data.xml.xsl.xsltprocessor.aspx)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
