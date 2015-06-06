Direct2D gradient mesh sample
=============================

This sample demonstrates how to instantiate and render a gradient mesh in [Direct2D](http://msdn.microsoft.com/en-us/library/windows/desktop/dd370990). This samples uses the helper method provided by Direct2D to create a mesh consisting of two tensor patches that share a side.

Some important APIs used in this sample are:

- The [**D2D1::GradientMeshPatch**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890770) function, which creates the [**D2D1\_GRADIENT\_MESH\_PATCH**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890726) structures that contain the tensor patch data.
- The [**ID2D1DeviceContext2::CreateGradientMesh**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890790) method, which creates an [**ID2D1GradientMesh**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn900410) object on the Direct2D device.
- The [**ID2D1DeviceContext2::DrawGradientMesh**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn900378) method, which renders the ID2D1GradientMesh object to the device context.

Related topics
--------------

[**D2D1\_GRADIENT\_MESH\_PATCH** structure](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890726)