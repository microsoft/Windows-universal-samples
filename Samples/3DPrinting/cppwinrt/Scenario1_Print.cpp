//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario1_Print.h"
#include "Scenario1_Print.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Graphics::Printing3D;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;

#pragma region Creating a package programmatically
namespace
{
#pragma region Buffer, stream, and string helpers
    void SetBufferBytes(IBuffer const& buffer, void const* data, uint32_t size)
    {
        memcpy_s(buffer.data(), buffer.Length(), data, size);
    }

    IAsyncOperation<hstring> StreamToStringAsync(IRandomAccessStream stream)
    {
        uint32_t size = static_cast<uint32_t>(stream.Size());
        Buffer buffer(size);
        IBuffer readBuffer = co_await stream.ReadAsync(buffer, size, InputStreamOptions::None);
        co_return CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, readBuffer);
    }

    IAsyncOperation<IRandomAccessStream> StringToStreamAsync(hstring const& string)
    {
        IBuffer buffer = CryptographicBuffer::ConvertStringToBinary(string, BinaryStringEncoding::Utf8);
        InMemoryRandomAccessStream outputStream;
        co_await outputStream.WriteAsync(buffer);
        co_return outputStream;
    }
#pragma endregion

#pragma region Mesh buffers
    // Create the buffer for vertex positions.
    void SetVertices(Printing3DMesh const& mesh)
    {
        Printing3DBufferDescription description;
        description.Format = Printing3DBufferFormat::Printing3DDouble;
        description.Stride = 3; // Three values per vertex (x, y, z).
        mesh.VertexPositionsDescription(description);
        mesh.VertexCount(8); // 8 total vertices in the cube

        // Create the buffer into which we will write the vertex positions.
        mesh.CreateVertexPositions(sizeof(double) * description.Stride * mesh.VertexCount());

        // Fill the buffer with vertex coordinates.
        static constexpr double vertices[] =
        {
            0, 0, 0,
            10, 0, 0,
            0, 10, 0,
            10, 10, 0,
            0, 0, 10,
            10, 0, 10,
            0, 10, 10,
            10, 10, 10,
        };

        SetBufferBytes(mesh.GetVertexPositions(), vertices, sizeof(vertices));
    }

    // Create the buffer for triangle indices.
    void SetTriangleIndices(Printing3DMesh const& mesh)
    {
        Printing3DBufferDescription description;
        description.Format = Printing3DBufferFormat::Printing3DUInt;
        description.Stride = 3; // 3 vertex position indices per triangle
        mesh.IndexCount(12); // 12 triangles in the cube
        mesh.TriangleIndicesDescription(description);

        // Create the buffer into which we will write the triangle vertex indices.
        mesh.CreateTriangleIndices(sizeof(uint32_t) * description.Stride * mesh.IndexCount());

        // Fill the buffer with triangle vertex indices.
        static constexpr uint32_t indices[] =
        {
            1, 0, 2,
            1, 2, 3,
            0, 1, 5,
            0, 5, 4,
            1, 3, 7,
            1, 7, 5,
            2, 7, 3,
            2, 6, 7,
            0, 6, 2,
            0, 4, 6,
            6, 5, 7,
            4, 5, 6,
        };

        SetBufferBytes(mesh.GetTriangleIndices(), indices, sizeof(indices));
    }


    // Create the buffer for material indices.
    void SetMaterialIndices(Printing3DMesh const& mesh)
    {
        Printing3DBufferDescription description;
        description.Format = Printing3DBufferFormat::Printing3DUInt;
        description.Stride = 4; // 4 indices per material
        mesh.IndexCount(12); // 12 triangles with material
        mesh.TriangleMaterialIndicesDescription(description);

        // Create the buffer into which we will write the material indices.
        mesh.CreateTriangleMaterialIndices(sizeof(uint32_t) * description.Stride * mesh.IndexCount());

        // Fill the buffer with material indices.
        static constexpr uint32_t indices[] =
        {
            // base materials:
            // in  the BaseMaterialGroup, the BaseMaterial with id=0 will be applied to these triangle vertices
            2, 0, 0, 0,
            2, 0, 0, 0,
            // color materials:
            // in the ColorMaterialGroup, the ColorMaterials with these ids will be applied to these triangle vertices
            1, 1, 1, 1,
            1, 1, 1, 1,
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 1, 0,
            1, 0, 1, 1,
            // composite materials:
            // in the CompositeMaterialGroup, the CompositeMaterial with id=0 will be applied to these triangles
            3,0,0,0,
            3,0,0,0,
            // texture materials:
            // in the Texture2CoordMaterialGroup, each texture coordinate is mapped to the appropriate vertex on these
            // two adjacent triangle faces, so that the square face they create displays the original rectangular image
            4, 0, 3, 1,
            4, 2, 3, 0,
        };

        SetBufferBytes(mesh.GetTriangleMaterialIndices(), indices, sizeof(indices));
    }
#pragma endregion

    // Works around an issue with Printing3DTextureResource: It does not respect
    // the TextureData.ContentType property, and always encodes the content type as "".
    // Repair the XML by assuming that a path that ends with ".png" is a PNG file,
    // and everything else is a JPG file.
    IAsyncAction FixTextureContentTypeAsync(Printing3D3MFPackage package)
    {
        XmlDocument xmldoc;
        xmldoc.LoadXml(co_await StreamToStringAsync(package.ModelPart()));
        hstring ns3d = L"xmlns:m='http://schemas.microsoft.com/3dmanufacturing/material/2015/02'";

        for (IXmlNode node : xmldoc.SelectNodesNS(L"//m:texture2d[@contenttype='']", box_value(ns3d)))
        {
            IXmlNode contentType = node.Attributes().GetNamedItem(L"contenttype");
            auto path = unbox_value<hstring>(node.Attributes().GetNamedItem(L"path").NodeValue());
            std::wstring_view path_view = path;
            if (path_view.size() >= 4 && path_view.substr(path_view.size() - 4) == L".png")
            {
                contentType.NodeValue(box_value(L"image/png"));
            }
            else
            {
                contentType.NodeValue(box_value(L"image/jpg"));
            }
        }

        // Update the model with the repaired XML.
        package.ModelPart(co_await StringToStreamAsync(xmldoc.GetXml()));
    }

    IAsyncOperation<Printing3D3MFPackage> CreatePackageAsync()
    {
        Printing3D3MFPackage package;

        Printing3DModel model;
        model.Unit(Printing3DModelUnit::Millimeter);

        // Material indices start at 1. (0 is reserved.)

#pragma region Color materials
        // Create color materials.
        Printing3DColorMaterial colorMaterial1;
        colorMaterial1.Color({ 255, 20, 20, 90 });

        Printing3DColorMaterial colorMaterial2;
        colorMaterial2.Color({ 255, 250, 120, 45 });

        // Create a color group with id 1 and add the colors to it.
        Printing3DColorMaterialGroup colorGroup(1);
        colorGroup.Colors().ReplaceAll({ colorMaterial1, colorMaterial2 });

        // add the color group to the model.
        model.Material().ColorGroups().Append(colorGroup);
#pragma endregion

#pragma region Base materials
        // Create base materials.
        Printing3DBaseMaterial material1;
        material1.Name(Printing3DBaseMaterial::Pla());
        material1.Color(colorMaterial1);

        Printing3DBaseMaterial material2;
        material2.Name(Printing3DBaseMaterial::Abs());
        material2.Color(colorMaterial2);

        // Create a new base material group with id 2 and add the base materials to it.
        Printing3DBaseMaterialGroup materialGroup(2);
        materialGroup.Bases().ReplaceAll({ material1, material2 });

        // Add the material group to the base groups on the model.
        model.Material().BaseGroups().Append(materialGroup);

#pragma endregion

#pragma region Composite material groups
        // Create a composite material group with id 3.
        Printing3DCompositeMaterialGroup compositeMaterialGroup(3);

        // Add it to the metadata.
        // The key is the string "composite" followed by the composite material group id.
        // The value specifies that the default base material group to use is id 2.
        model.Metadata().Insert(L"composite3", L"2");

        // The indices are relative to the material indices that will be set by SetMaterialIndicesAsync.
        compositeMaterialGroup.MaterialIndices().ReplaceAll({ 0, 1 });

        // Create new composite materials.
        // The Values correspond to the materials added in the compositeMaterialGroup,
        // and they must sum to 1.0.

        // Composite material 1 consists of 20% material1 and 80% material2.
        Printing3DCompositeMaterial compositeMaterial1;
        compositeMaterial1.Values().ReplaceAll({ 0.2, 0.8 });

        // Composite material 2 consists of 50% material1 and 50% material2.
        Printing3DCompositeMaterial compositeMaterial2;
        compositeMaterial2.Values().ReplaceAll({ 0.5, 0.5 });

        // Composite material 3 consists of 80% material1 and 20% material2.
        Printing3DCompositeMaterial compositeMaterial3;
        compositeMaterial3.Values().ReplaceAll({ 0.8, 0.2 });

        // Composite material 4 consists of 40% material1 and 60% material2.
        Printing3DCompositeMaterial compositeMaterial4;
        compositeMaterial4.Values().ReplaceAll({ 0.4, 0.6 });

        // Add the composite materials to the compositeMaterialGroup.
        compositeMaterialGroup.Composites().ReplaceAll({ compositeMaterial1, compositeMaterial2, compositeMaterial3, compositeMaterial4 });

        // Add the composite material group to the model.
        model.Material().CompositeGroups().Append(compositeMaterialGroup);
#pragma endregion

#pragma region Texture resource
        Printing3DTextureResource textureResource;

        // Set the texture path in the 3MF file.
        textureResource.Name(L"/3D/Texture/msLogo.png");

        // Load the texture from our package.
        StorageFile file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri(L"ms-appx:///Assets/msLogo.png"));
        textureResource.TextureData(co_await file.OpenReadAsync());

        package.Textures().Append(textureResource);
#pragma endregion

#pragma region 2D texture materials
        // Create a texture material group with id 4.
        Printing3DTexture2CoordMaterialGroup texture2CoordGroup(4);

        // Add it to the metadata.
        // The key is the string "tex" followed by the texture material group id.
        // The value is the name of the texture resource (see textureResource.Name above).
        model.Metadata().Insert(L"tex4", L"/3D/Texture/msLogo.png");

        // Create texture materials and add them to the group.
        Printing3DTexture2CoordMaterial coord1;
        coord1.U(0.0);
        coord1.V(1.0);

        Printing3DTexture2CoordMaterial coord2;
        coord2.U(1.0);
        coord2.V(1.0);

        Printing3DTexture2CoordMaterial coord3;
        coord3.U(0.0);
        coord3.V(0.0);

        Printing3DTexture2CoordMaterial coord4;
        coord4.U(1.0);
        coord4.V(0.0);

        texture2CoordGroup.Texture2Coords().ReplaceAll({ coord1, coord2, coord3, coord4 });

        // Add the texture material group to the model.
        model.Material().Texture2CoordGroups().Append(texture2CoordGroup);
#pragma endregion

#pragma region Mesh
        Printing3DMesh mesh;

        SetVertices(mesh);
        SetTriangleIndices(mesh);
        SetMaterialIndices(mesh);

        // add the mesh to the model
        model.Meshes().Append(mesh);
#pragma endregion

#pragma region Adding a component to the build
        // create a component.
        Printing3DComponent component;

        // assign the mesh to the component's mesh.
        component.Mesh(mesh);

        // Add the component to the model. A model can have multiple components.
        model.Components().Append(component);

        // The world matrix for the component is the identity matrix.
        Printing3DComponentWithMatrix componentWithMatrix;
        componentWithMatrix.Component(component);
        componentWithMatrix.Matrix(float4x4::identity());

        // add the componentWithMatrix to the build.
        // The build defines what is to be printed from within a Printing3DModel.
        // If you leave a mesh out of the build, it will not be printed.
        model.Build().Components().Append(componentWithMatrix);
#pragma endregion

        // Save the completed model into a package.
        co_await package.SaveModelToPackageAsync(model);

        // fix any textures in the model file.
        co_await FixTextureContentTypeAsync(package);

        co_return package;
    }

    IAsyncOperation<Printing3D3MFPackage> CreatePackageFromXmlAsync(hstring rawXml)
    {
        Printing3D3MFPackage package;
        package.ModelPart(co_await StringToStreamAsync(rawXml));
        co_return package;
    }
}
#pragma endregion

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Print::Scenario1_Print()
    {
        InitializeComponent();
    }

    void Scenario1_Print::EnablePackageOperationButtons()
    {
        SaveButton().IsEnabled(true);
        PrintButton().IsEnabled(true);
    }

    fire_and_forget Scenario1_Print::CreateProgrammatically_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        currentPackage = co_await CreatePackageAsync();
        rootPage.NotifyUser(L"Package created programmatically.", NotifyType::StatusMessage);
        EnablePackageOperationButtons();
    }

    fire_and_forget Scenario1_Print::CreateFromXml_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        static constexpr wchar_t rawXml[] = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            L"<model unit=\"millimeter\" xml:lang=\"en-US\" xmlns=\"http://schemas.microsoft.com/3dmanufacturing/2013/01\">"
            L"<resources><object id=\"0\" type=\"model\"><mesh><vertices>"
            L"<vertex x=\"0.000000\" y=\"0.000000\" z=\"10.000000\" />"
            L"<vertex x=\"0.000000\" y=\"10.000000\" z=\"10.000000\" />"
            L"<vertex x=\"0.000000\" y=\"0.000000\" z=\"0.000000\" />"
            L"<vertex x=\"0.000000\" y=\"10.000000\" z=\"0.000000\" />"
            L"<vertex x=\"10.000000\" y=\"0.000000\" z=\"10.000000\" />"
            L"<vertex x=\"10.000000\" y=\"10.000000\" z=\"10.000000\" />"
            L"<vertex x=\"10.000000\" y=\"0.000000\" z=\"0.000000\" />"
            L"<vertex x=\"10.000000\" y=\"10.000000\" z=\"0.000000\" />"
            L"</vertices><triangles>"
            L"<triangle v1=\"0\" v2=\"1\" v3=\"2\" />"
            L"<triangle v1=\"2\" v2=\"1\" v3=\"3\" />"
            L"<triangle v1=\"0\" v2=\"4\" v3=\"5\" />"
            L"<triangle v1=\"1\" v2=\"0\" v3=\"5\" />"
            L"<triangle v1=\"2\" v2=\"6\" v3=\"4\" />"
            L"<triangle v1=\"0\" v2=\"2\" v3=\"4\" />"
            L"<triangle v1=\"3\" v2=\"7\" v3=\"2\" />"
            L"<triangle v1=\"2\" v2=\"7\" v3=\"6\" />"
            L"<triangle v1=\"1\" v2=\"5\" v3=\"3\" />"
            L"<triangle v1=\"3\" v2=\"5\" v3=\"7\" />"
            L"<triangle v1=\"6\" v2=\"7\" v3=\"5\" />"
            L"<triangle v1=\"4\" v2=\"6\" v3=\"5\" />"
            L"</triangles></mesh></object></resources>"
            L"<build><item objectid=\"0\" transform=\"10 0 0 0 10 0 0 0 10 0 0 0\" /></build></model>";
        currentPackage = co_await CreatePackageFromXmlAsync(rawXml);
        rootPage.NotifyUser(L"Package created from XML.", NotifyType::StatusMessage);
        EnablePackageOperationButtons();
    }

    fire_and_forget Scenario1_Print::LoadPackageFromFile_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        FileOpenPicker openPicker;
        openPicker.ViewMode(PickerViewMode::Thumbnail);
        openPicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        openPicker.FileTypeFilter().Append(L".3mf");

        StorageFile file = co_await openPicker.PickSingleFileAsync();
        if (file == nullptr)
        {
            co_return;
        }

        rootPage.NotifyUser(L"Loading...", NotifyType::StatusMessage);

        Printing3D3MFPackage package;
        IRandomAccessStream fileStream = co_await file.OpenAsync(FileAccessMode::Read);
        Printing3DModel model = co_await package.LoadModelFromPackageAsync(fileStream);
        rootPage.NotifyUser(L"Repairing...", NotifyType::StatusMessage);
        co_await model.RepairAsync();
        rootPage.NotifyUser(L"Saving...", NotifyType::StatusMessage);
        co_await package.SaveModelToPackageAsync(model);
        co_await FixTextureContentTypeAsync(package);
        fileStream.Close();

        currentPackage = package;
        rootPage.NotifyUser(L"Package created from file.", NotifyType::StatusMessage);
        EnablePackageOperationButtons();
    }

    fire_and_forget Scenario1_Print::SavePackage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        FileSavePicker savePicker;
        savePicker.DefaultFileExtension(L".3mf");
        savePicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        savePicker.FileTypeChoices().Insert(L"3MF File", single_threaded_vector<hstring>({ L".3mf" }));
        StorageFile storageFile = co_await savePicker.PickSaveFileAsync();
        if (storageFile == nullptr)
        {
            co_return;
        }

        IRandomAccessStream stream = co_await currentPackage.SaveAsync();
        stream.Seek(0);
        DataReader dataReader(stream);
        auto size = (uint32_t)stream.Size();
        co_await dataReader.LoadAsync(size);
        IBuffer buffer = dataReader.ReadBuffer(size);
        co_await FileIO::WriteBufferAsync(storageFile, buffer);
        stream.Close();

        rootPage.NotifyUser(L"Saved", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario1_Print::PrintPackage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // register the PrintTaskRequest callback to get the Printing3D3MFPackage.
        auto print3DManager = Print3DManager::GetForCurrentView();
        event_token taskRequestedToken = print3DManager.TaskRequested({ this, &Scenario1_Print::OnTaskRequested });

        // show the PrintUI
        [[maybe_unused]] bool result = co_await Print3DManager::ShowPrintUIAsync();

        // Remove the print task request after the dialog is complete.
        print3DManager.TaskRequested(taskRequestedToken);
    }

    void Scenario1_Print::OnTaskRequested(Print3DManager const&, Print3DTaskRequestedEventArgs const& args)
    {
        Print3DTask print3dTask = args.Request().CreateTask(L"Sample Model", L"Default", [this](auto&& e) { e.SetSource(currentPackage); });

        // Optional notification handlers.
        print3dTask.Completed([rootPage = rootPage](auto&&, auto&&) { rootPage.NotifyUser(L"Printing completed.", NotifyType::StatusMessage); });
        print3dTask.Submitting([rootPage = rootPage](auto&&, auto&&) { rootPage.NotifyUser(L"Submitting print job.", NotifyType::StatusMessage); });
    }

}
