using UnityEngine;
using System.Collections;
using System;
using System.IO;
#if UNITY_WSA && !UNITY_EDITOR
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Text;
using Windows.ApplicationModel.Core;
using Windows.Graphics.Printing3D;
using Windows.UI.Core;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
#endif

public class Print : MonoBehaviour
{
    private static string Step1 = "Get mesh/texture from Game Object";
    private static string Step2 = "Store into Printing3d API";
    private static string Step3 = "Launch printing dialog";
    private static string labelText = "Status for printing";

    private static bool enableButton = true;

    private static UInt32[] indicesPrint;
    private static double[] verticesPrint;
    private static UInt32[] indicesMaterialPrint;

    private static UInt32 groupId;
    private static double[] uvPrint;

    private static byte[] pngBytes;

    private static uint indicesCount;
    private static uint verticesCount;
    private static uint verticesMaterialCount;

    // Use this for adding GUI
    void OnGUI()
    {
        GUI.Label(new UnityEngine.Rect(20, 60, 300, 40), labelText);
        if (GUI.Button(new UnityEngine.Rect(20, 20, 70, 25), "3D Print"))
        {
            // ignore button click until launch print dialog
            if (enableButton)
            {
                enableButton = false;
            }
            else
            {
                return;
            }

            labelText = Step1;
            // get texture bytes from Unity
            var texture = transform.GetComponent<SkinnedMeshRenderer>().material.GetTexture("_MainTex") as Texture2D;
            pngBytes = texture.EncodeToPNG();

            // get mesh information from Unity
            var mesh = transform.GetComponent<SkinnedMeshRenderer>().sharedMesh;
            indicesCount = (uint)mesh.triangles.Length / 3;
            verticesCount = (uint)mesh.vertices.Length;
            verticesMaterialCount = (uint)mesh.triangles.Length / 3;

            indicesPrint = new UInt32[mesh.triangles.Length];
            verticesPrint = new double[mesh.vertices.Length * 3];
            indicesMaterialPrint = new UInt32[mesh.triangles.Length / 3 * 4];
            uvPrint = new double[mesh.uv.Length * 2];

            groupId = 1;

            int j = 0;

            // get indices and indices for material
            for (int i = 0; i < mesh.triangles.Length; i++)
            {
                indicesPrint[i] = (UInt32)mesh.triangles[i];

                // vertex corresponding to uv
                if (i % 3 == 0)
                {
                    indicesMaterialPrint[j] = groupId;
                    j++;
                }

                indicesMaterialPrint[j] = (UInt32)mesh.triangles[i];
                j++;
            }

            // get vertices
            for (int i = 0; i < mesh.vertices.Length; i++)
            {
                verticesPrint[i * 3] = (double)mesh.vertices[i].x;
                verticesPrint[i * 3 + 1] = (double)mesh.vertices[i].y;
                verticesPrint[i * 3 + 2] = (double)mesh.vertices[i].z;
            }

            // get UVs
            for (int i = 0; i < mesh.uv.Length; i++)
            {
                uvPrint[i * 2] = (double)mesh.uv[i].x;
                uvPrint[i * 2 + 1] = (double)mesh.uv[i].y;
            }

#if UNITY_WSA && !UNITY_EDITOR
            // start to pass the mesh/texture into Printing3D API
            // launch the print dialog
            UIThread();
#endif
        }
    }

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }

#if UNITY_WSA && !UNITY_EDITOR
    private static async void UIThread()
    {
        // convert to UI thread and then launch the printing dialog
        // print dialog only works in UI thread
        await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
            var print = new Print();
            print.LaunchPrintDialog();
        });
    }

    private Print3DTask printTask;
    private Printing3D3MFPackage package;

    private async void LaunchPrintDialog()
    {
        labelText = Step2;
        // use CreateData to create 3MFPackage from a string stream
        await CreateData();

        labelText = Step3;
        // register the PrintTaskRequest callback to get the Printing3D3MFPackage
        Print3DManager.GetForCurrentView().TaskRequested += MainPage_TaskRequested;

        // show the PrintUI
        await Print3DManager.ShowPrintUIAsync();

        // remove the print task request after dialog is shown            
        Print3DManager.GetForCurrentView().TaskRequested -= MainPage_TaskRequested;

        // set back
        enableButton = true;
    }

    private void MainPage_TaskRequested(Print3DManager sender, Print3DTaskRequestedEventArgs args)
    {
        Print3DTask printTaskRef = null;

        Print3DTaskSourceRequestedHandler sourceHandler = delegate (Print3DTaskSourceRequestedArgs sourceRequestedArgs)
        {
            sourceRequestedArgs.SetSource(package);
        };
        printTaskRef = args.Request.CreateTask("Model", "Default", sourceHandler);

        this.printTask = printTaskRef;

        printTaskRef.Completed += Task_Completed;
        printTaskRef.Submitting += Task_Submitting;
    }

    private async Task CreateData()
    {            
        package = new Printing3D3MFPackage();
           
        var model = new Printing3DModel();

        model.Unit = Printing3DModelUnit.Millimeter;

        var mesh = new Printing3DMesh();
    
        // save vertices
        await GetVerticesAsync(mesh);

        // save indices
        await GetIndicesAsync(mesh);
            
        // save material indices
        await GetMaterialIndicesAsync(mesh);

        // texture2Coord Group
        var tex2CoordGroup = new Printing3DTexture2CoordMaterialGroup(groupId);
            
        // save texture coords
        for (int i = 0; i < uvPrint.Length / 2; i++)
        {
            var tex2CoordMaterial = new Printing3DTexture2CoordMaterial();
            tex2CoordMaterial.U = uvPrint[i * 2];
            tex2CoordMaterial.V = uvPrint[i * 2 + 1];
            tex2CoordGroup.Texture2Coords.Add(tex2CoordMaterial);
        }
    
        // to make sure we don't use the same byte array from Unity
        var copyPngBytes = new byte[pngBytes.Length];
        pngBytes.CopyTo(copyPngBytes, 0);
 
        var randomAccessStream = new InMemoryRandomAccessStream();
        await randomAccessStream.WriteAsync(copyPngBytes.AsBuffer());
        randomAccessStream.Seek(0); 

        var texture = new Printing3DModelTexture();
        Printing3DTextureResource texResource = new Printing3DTextureResource();
        texResource.Name = "/3D/Texture/skeleton.png";
        texResource.TextureData = new MyRandomAccessStream(randomAccessStream);
        package.Textures.Add(texResource);

        // add metadata about the texture
        model.Metadata.Add("tex" + groupId, "/3D/Texture/skeleton.png");
        model.Material.Texture2CoordGroups.Add(tex2CoordGroup);

        model.Meshes.Add(mesh);

        Printing3DComponent component = new Printing3DComponent();

        component.Mesh = mesh;

        model.Components.Add(component);

        var componentWithMatrix = new Printing3DComponentWithMatrix();

        componentWithMatrix.Component = component;

        componentWithMatrix.Matrix = System.Numerics.Matrix4x4.Identity;

        model.Build.Components.Add(componentWithMatrix);

        await package.SaveModelToPackageAsync(model);
		
        var modelStream = package.ModelPart;
        package.ModelPart = await FixTextureContentType(modelStream);
		
        // save to file and easy to debug
        // var stream = await package.SaveAsync();
        // await SaveStreamTo3MF(stream);
    }
        
    private async Task<IRandomAccessStream> FixTextureContentType(IRandomAccessStream modelStream)
    {
        XDocument xmldoc = XDocument.Load(modelStream.AsStreamForRead());

        var outputStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
        var writer = new Windows.Storage.Streams.DataWriter(outputStream);
        writer.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
        writer.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

        var text = xmldoc.ToString();

        var replacedText = text.Replace("contenttype=\"\"", "contenttype=\"image/png\"");
        writer.WriteString(replacedText);

        await writer.StoreAsync();
        await writer.FlushAsync();
        writer.DetachStream();
        return outputStream;
    }
		
    private async static Task<bool> SaveStreamTo3MF(Windows.Storage.Streams.IRandomAccessStream stream)
    {
        // set back
        stream.Seek(0);
        using (var dataReader = new Windows.Storage.Streams.DataReader(stream))
        {

            var dataLoad = await dataReader.LoadAsync((uint)stream.Size);
            if (dataLoad > 0)
            {
                var buffer = dataReader.ReadBuffer((uint)stream.Size);                    

                StorageFolder localFolder = ApplicationData.Current.LocalFolder;
                var outputfile = await localFolder.CreateFileAsync("output.3mf", CreationCollisionOption.ReplaceExisting);
                await Windows.Storage.FileIO.WriteBufferAsync(outputfile, buffer);
                var options = new Windows.System.LauncherOptions();
                options.TargetApplicationPackageFamilyName = "Microsoft.3DBuilder_8wekyb3d8bbwe";
            }
        }

        return true;
    }

    private void Task_Submitting(Print3DTask sender, object args)
    {
        // notify user if required
    }

    private void Task_Completed(Print3DTask sender, Print3DTaskCompletedEventArgs args)
    {
        // notify user if required
    }

    private async Task GetVerticesAsync(Printing3DMesh mesh)
    {
        Printing3DBufferDescription description;

        description.Format = Printing3DBufferFormat.Printing3DDouble;
        description.Stride = 3;    
        mesh.VertexCount = verticesCount;
        mesh.CreateVertexPositions(sizeof(double) * 3 * mesh.VertexCount);
        mesh.VertexPositionsDescription = description;

        using (var stream = mesh.GetVertexPositions().AsStream())
        {
            var data = verticesPrint.SelectMany(v => BitConverter.GetBytes(v)).ToArray();
            await stream.WriteAsync(data, 0, data.Length);
        }
    }

    private static async Task GetIndicesAsync(Printing3DMesh mesh)
    {
        Printing3DBufferDescription description;

        description.Format = Printing3DBufferFormat.Printing3DUInt;
        description.Stride = 3;
        mesh.IndexCount = indicesCount;
        mesh.TriangleIndicesDescription = description;

        mesh.CreateTriangleIndices(sizeof(UInt32) * 3 * mesh.IndexCount);

        var stream = mesh.GetTriangleIndices().AsStream();
        {
            var data = indicesPrint.SelectMany(v => BitConverter.GetBytes(v)).ToArray();
            await stream.WriteAsync(data, 0, data.Length);
        }

    }

    private static async Task GetMaterialIndicesAsync(Printing3DMesh mesh)
    {
        Printing3DBufferDescription description;

        description.Format = Printing3DBufferFormat.Printing3DUInt;
        description.Stride = 4;
        mesh.IndexCount = verticesMaterialCount;
        mesh.TriangleMaterialIndicesDescription = description;

        mesh.CreateTriangleMaterialIndices(sizeof(UInt32) * 4 * mesh.IndexCount);

        var stream = mesh.GetTriangleMaterialIndices().AsStream();
        {
            var data = indicesMaterialPrint.SelectMany(v => BitConverter.GetBytes(v)).ToArray();
            await stream.WriteAsync(data, 0, data.Length);
        }
    }

    // create one IRandomAccessStreamWithContentType class
    // in WinRT API, there is no instance for IRandomAccessStreamWithContentType
    // for the user, you just need to copy this class, change the value for "ContentType"
    public class MyRandomAccessStream : IRandomAccessStreamWithContentType
    {
        private IRandomAccessStream _stream;

        public bool CanRead
        {
            get
            {
                return _stream.CanRead;
            }
        }

        public bool CanWrite
        {
            get
            {
                return _stream.CanWrite;
            }
        }

        public ulong Position
        {
            get
            {
                return _stream.Position;
            }
        }

        public ulong Size
        {
            get
            {
                return _stream.Size;
            }

            set
            {
                _stream.Size = value;
            }
        }

        public string ContentType
        {
            get
            {
                // depending on the real data type
                return "image/png";
            }
        }

        public MyRandomAccessStream(IRandomAccessStream stream)
        {
            _stream = stream;
        }

        public IRandomAccessStream CloneStream()
        {
            return _stream.CloneStream();
        }

        public void Dispose()
        {
            _stream.Dispose();
        }

        public IAsyncOperation<bool> FlushAsync()
        {
            return _stream.FlushAsync();
        }

        public IInputStream GetInputStreamAt(ulong position)
        {
            return _stream.GetInputStreamAt(position);
        }

        public IOutputStream GetOutputStreamAt(ulong position)
        {
            return _stream.GetOutputStreamAt(position);
        }

        public IAsyncOperationWithProgress<IBuffer, UInt32> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
        {
            return _stream.ReadAsync(buffer, count, options);
        }

        public void Seek(ulong position)
        {
            _stream.Seek(position);
        }

        public IAsyncOperationWithProgress<UInt32, UInt32> WriteAsync(IBuffer buffer)
        {
            return _stream.WriteAsync(buffer);
        }
    }
#endif
}
