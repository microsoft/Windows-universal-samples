using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Graphics.Printing3D;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.System.Threading;
using System.Threading.Tasks;
using System.Numerics;
using System.Xml.Linq;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Printing3DSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private Print3DTask printTask;
        private Printing3D3MFPackage package;
        private IRandomAccessStream stream;
        private StorageFile file;

        public MainPage()
        {
            this.InitializeComponent();
        }

        // Save the IRandonAccessStream
        public void SaveStream(IRandomAccessStream streamS)
        {
            this.stream = streamS;
        }

        // Save the Printing3D3MFPackage
        public void SavePackage(Printing3D3MFPackage pack)
        {
            this.package = pack;

        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// Launch the 3D Print Dialog with a 3D model created from the in-memory 
        /// Printing3DModel. In this sample we create a 3D cube with different material. 
        /// This cube is then submitted for 3D Printing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint(object sender, RoutedEventArgs e)
        {
            // use CreateData() to create 3MFPackage using the inmemory printing3dmodel
            await CreateData();

            // register the PrintTaskRequest callback to get the Printing3D3MFPackage
            Print3DManager.GetForCurrentView().TaskRequested += MainPage_TaskRequested;
            // show the PrintUI
            var result = await Print3DManager.ShowPrintUIAsync();

            // remove the print task request after dialog is shown            
            Print3DManager.GetForCurrentView().TaskRequested -= MainPage_TaskRequested;
        }


        /// <summary>
        /// Launch the 3D Print Dialog with a 3D Model created from an xml model file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint2(object sender, RoutedEventArgs e)
        {
            // use Create3MFData to create 3MFPackage from an xml stream
            await Create3MFData();

            // register the PrintTaskRequest callback to get the Printing3D3MFPackage
            Print3DManager.GetForCurrentView().TaskRequested += MainPage_TaskRequested;
            // show the PrintUI
            var result = await Print3DManager.ShowPrintUIAsync();

            // remove the print task request after dialog is shown            
            Print3DManager.GetForCurrentView().TaskRequested -= MainPage_TaskRequested;
        }

        /// <summary>
        /// Launch 3D Builder with an input file using file activation
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint3(object sender, RoutedEventArgs e)
        {
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            openPicker.FileTypeFilter.Add(".3mf");
            openPicker.FileTypeFilter.Add(".stl");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file == null)
            {
                return;
            }

            var options = new Windows.System.LauncherOptions();
            // setup 3D Builder as the target for this file launch
            options.TargetApplicationPackageFamilyName = "Microsoft.3DBuilder_8wekyb3d8bbwe";

            // Launch the retrieved file
            var success = await Windows.System.Launcher.LaunchFileAsync(file, options);
        }

        /// <summary>
        /// Load a 3MF file, repair the file and then submit the file 
        /// for 3D Printing.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint4(object sender, RoutedEventArgs e)
        {
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            openPicker.FileTypeFilter.Add(".3mf");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file == null)
            {
                return;
            }
            var fileStream = await file.OpenAsync(FileAccessMode.Read);

            /// update status message
            OutputTextBlock.Text = "\nLoading";
            package = new Printing3D3MFPackage();
            var model = await package.LoadModelFromPackageAsync(fileStream);
            // update status message
            OutputTextBlock.Text = "\nRepairing";
            // repair model
            await model.RepairAsync();

            OutputTextBlock.Text = "\nSaving Model";
            await package.SaveModelToPackageAsync(model);
            OutputTextBlock.Text = "\nPrinting";

            // fix contenttype if file has any textures
            package.ModelPart = await FixTextureContentType(package.ModelPart);

            // save the package
            SavePackage(package);

            // register the PrintTaskRequest callback to get the Printing3D3MFPackage
            Print3DManager.GetForCurrentView().TaskRequested += MainPage_TaskRequested;
            // show the PrintUI
            var result = await Print3DManager.ShowPrintUIAsync();

            // remove the print task request after dialog is shown            
            Print3DManager.GetForCurrentView().TaskRequested -= MainPage_TaskRequested;
            // clear status message
            OutputTextBlock.Text = "\n";
        }

        /// <summary>
        /// Sample shows how to save a Printing3D3MFPackage to a file
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint5(object sender, RoutedEventArgs e)
        {
            // use CreateData() to create 3MFPackage using the inmemory printing3dmodel
            await CreateData();

            var stream = await package.SaveAsync();


            FileSavePicker savePicker = new FileSavePicker();
            savePicker.DefaultFileExtension = ".3mf";
            savePicker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.DocumentsLibrary;
            savePicker.FileTypeChoices.Add("3MF File", new List<string>() { ".3mf" });
            var storageFile = await savePicker.PickSaveFileAsync();
            // saves IRandomAccessStream to the user picked location
            await saveStreamTo3MF(stream, storageFile);
        }


        /// <summary>
        /// Save a Printing3D3MFPackage to disk and activate 3DBuilder to load the file
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint6(object sender, RoutedEventArgs e)
        {
            // use CreateData() to create 3MFPackage using the inmemory printing3dmodel
            await CreateData();

            var stream = await package.SaveAsync();
            await saveStreamTo3MF(stream);

            StorageFolder localFolder = ApplicationData.Current.LocalFolder;
            var filePath = Path.Combine(localFolder.Path, "output.3mf");

            StorageFile localFile = await StorageFile.GetFileFromPathAsync(filePath);

            var options = new Windows.System.LauncherOptions();
            options.TargetApplicationPackageFamilyName = "Microsoft.3DBuilder_8wekyb3d8bbwe";

            // Launch the retrieved file
            var success = await Windows.System.Launcher.LaunchFileAsync(localFile, options);

        }

        /// <summary>
        /// Load a 3MF file and submit it for 3D Printing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint7(object sender, RoutedEventArgs e)
        {
            bool result;
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            openPicker.FileTypeFilter.Add(".3mf");

            file = await openPicker.PickSingleFileAsync();
            if (file == null)
            {
                return;
            }
            var fileStream = await file.OpenAsync(FileAccessMode.Read);
            // register the PrintTaskRequest callback to get the Printing3D3MFPackage
            Print3DManager.GetForCurrentView().TaskRequested += MainPage_TaskRequested;
            // the package.LoadAsync needs to be invoked in the same threading model as when setsource is performed which currently is in a background thread.
            var op = ThreadPool.RunAsync(async delegate { result = await Compute(fileStream); });
            // register the PrintTaskRequest callback to get the Printing3D3MFPackage
            // show the PrintUI
            result = await Print3DManager.ShowPrintUIAsync();
            Print3DManager.GetForCurrentView().TaskRequested -= MainPage_TaskRequested;
        }

        private async Task<bool> Compute(IRandomAccessStream fileStream)
        {
            package = await Printing3D3MFPackage.LoadAsync(fileStream);
            return true;
        }

        /// <summary>
        /// Sample which sends user to 3D printing documentation
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void On3DPrint8(object sender, RoutedEventArgs e)
        {
            try
            {
                Uri uri = new Uri(@"https://msdn.microsoft.com/en-us/windows/hardware/bg183398.aspx");
                var launched = await Windows.System.Launcher.LaunchUriAsync(uri);
            }
            catch (Exception)
            {
                //handle the exception
            }

        }

        /// <summary>
        /// Sets up a Printing3DTask for submitting a model for 3D Printing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void MainPage_TaskRequested(Print3DManager sender, Print3DTaskRequestedEventArgs args)
        {
            Print3DTask printTaskRef = null;

            Print3DTaskSourceRequestedHandler sourceHandler = delegate (Print3DTaskSourceRequestedArgs sourceRequestedArgs)
            {
                if (package != null)
                {
                    sourceRequestedArgs.SetSource(package);
                }
            };
            // create print3dtask
            printTaskRef = args.Request.CreateTask("Sample Model", "Default", sourceHandler);

            this.printTask = printTaskRef;

            // setup notification handlers
            printTaskRef.Completed += Task_Completed;
            printTaskRef.Submitting += Task_Submitting;
        }

        /// <summary>
        /// Sample creates a 3dmodel.model xml file and creates a 3mf file
        /// </summary>
        /// <returns>true when complete</returns>
        private async Task<bool> Create3MFData()
        {
            var stream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
            var writer = new Windows.Storage.Streams.DataWriter(stream);
            writer.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
            writer.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;

            writer.WriteString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            writer.WriteString("<model unit=\"millimeter\" xml:lang=\"en-US\" xmlns=\"http://schemas.microsoft.com/3dmanufacturing/2013/01\">");
            writer.WriteString("<resources><object id=\"0\" type=\"model\"><mesh><vertices>");
            writer.WriteString("<vertex x=\"0.000000\" y=\"0.000000\" z=\"10.000000\" />");
            writer.WriteString("<vertex x=\"0.000000\" y=\"10.000000\" z=\"10.000000\" />");
            writer.WriteString("<vertex x=\"0.000000\" y=\"0.000000\" z=\"0.000000\" />");
            writer.WriteString("<vertex x=\"0.000000\" y=\"10.000000\" z=\"0.000000\" />");
            writer.WriteString("<vertex x=\"10.000000\" y=\"0.000000\" z=\"10.000000\" />");
            writer.WriteString("<vertex x=\"10.000000\" y=\"10.000000\" z=\"10.000000\" />");
            writer.WriteString("<vertex x=\"10.000000\" y=\"0.000000\" z=\"0.000000\" />");
            writer.WriteString("<vertex x=\"10.000000\" y=\"10.000000\" z=\"0.000000\" />");
            writer.WriteString("</vertices><triangles>");
            writer.WriteString("<triangle v1=\"0\" v2=\"1\" v3=\"2\" />");
            writer.WriteString("<triangle v1=\"2\" v2=\"1\" v3=\"3\" />");
            writer.WriteString("<triangle v1=\"0\" v2=\"4\" v3=\"5\" />");
            writer.WriteString("<triangle v1=\"1\" v2=\"0\" v3=\"5\"  />");
            writer.WriteString("<triangle v1=\"2\" v2=\"6\" v3=\"4\"  />");
            writer.WriteString("<triangle v1=\"0\" v2=\"2\" v3=\"4\"  />");
            writer.WriteString("<triangle v1=\"3\" v2=\"7\" v3=\"2\"  />");
            writer.WriteString("<triangle v1=\"2\" v2=\"7\" v3=\"6\"  />");
            writer.WriteString("<triangle v1=\"1\" v2=\"5\" v3=\"3\"  />");
            writer.WriteString("<triangle v1=\"3\" v2=\"5\" v3=\"7\"  />");
            writer.WriteString("<triangle v1=\"6\" v2=\"7\" v3=\"5\"  />");
            writer.WriteString("<triangle v1=\"4\" v2=\"6\" v3=\"5\"  />");
            writer.WriteString("</triangles></mesh></object></resources>");
            writer.WriteString("<build><item objectid=\"0\" transform=\"10 0 0 0 10 0 0 0 10 0 0 0\" /></build></model>");
            await writer.StoreAsync();
            await writer.FlushAsync();
            writer.DetachStream();

            var package = new Printing3D3MFPackage();
            package.ModelPart = stream;
            // saves package
            SavePackage(package);
            return true;
        }

        /// <summary>
        /// Creates a Printing3D3MFPackage using in memory Printing3DModel APIs
        /// </summary>
        /// <returns>true when finished</returns>
        private async Task<bool> CreateData()
        {
            var localPackage = new Printing3D3MFPackage();
            var texture = new Printing3DModelTexture();
            Printing3DTextureResource texResource = new Printing3DTextureResource();
            // texture path in 3MF file after creation
            texResource.Name = "/3D/Texture/msLogo.png";
            // path to texture in the sample appx
            Uri texUri = new Uri("ms-appx:///Assets/msLogo.png");
            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(texUri);
            IRandomAccessStreamWithContentType iRandomAccessStreamWithContentType = await file.OpenReadAsync();

            texResource.TextureData = iRandomAccessStreamWithContentType;
            localPackage.Textures.Add(texResource);
            var model = new Printing3DModel();

            model.Unit = Printing3DModelUnit.Millimeter;

            // create new mesh on model
            var mesh = new Printing3DMesh();


            // add material group
            // all material indices need to start from 1
            // 0 is a reserved id
            // create new base materialgroup with id = 1
            var materialGroup = new Printing3DBaseMaterialGroup(1);

            // ARGB
            // A should be 255 if alpha = 100%
            var newColor = Windows.UI.Color.FromArgb(255, 20, 20, 90);
            var newColor2 = Windows.UI.Color.FromArgb(255, 250, 120, 45);
            var newColor3 = Windows.UI.Color.FromArgb(255, 1, 250, 200);

            // create new color materials
            var colrMat = new Printing3DColorMaterial();
            colrMat.Color = newColor;

            var colrMat2 = new Printing3DColorMaterial();
            colrMat2.Color = newColor2;

            var colrMat3 = new Printing3DColorMaterial();
            colrMat3.Color = newColor3;

            // setup new base materials
            var material = new Printing3DBaseMaterial
            {
                Name = Printing3DBaseMaterial.Pla,
                Color = colrMat
            };

            var material2 = new Printing3DBaseMaterial
            {
                Name = Printing3DBaseMaterial.Abs,
                Color = colrMat2
            };

            // add materials to the material group

            // material group index 0
            materialGroup.Bases.Add(material);
            // material group index 1
            materialGroup.Bases.Add(material2);

            // add material group to the basegroups on the model
            model.Material.BaseGroups.Add(materialGroup);


            // add colors to the color group
            var colorGroup = new Printing3DColorMaterialGroup(2);
            colorGroup.Colors.Add(colrMat);
            colorGroup.Colors.Add(colrMat2);
            colorGroup.Colors.Add(colrMat3);

            // add colorgroup to the colorgroups on the model
            model.Material.ColorGroups.Add(colorGroup);



            // CompositeGroups

            // create new composite material group
            var compGroup = new Printing3DCompositeMaterialGroup(3);
            // add to metadata
            // composite+id, "default base material to use is at index 1"            
            model.Metadata.Add("composite3", "1");
            // indices point to base materials in basematerialgroup with id =1
            compGroup.MaterialIndices.Add(0);
            compGroup.MaterialIndices.Add(1);
            // create new composite materials
            var compMat = new Printing3DCompositeMaterial();
            // fraction adds to 1.0
            compMat.Values.Add(0.2);
            compMat.Values.Add(0.8);

            var compMat2 = new Printing3DCompositeMaterial();
            // fraction adds to 1.0
            compMat2.Values.Add(0.5);
            compMat2.Values.Add(0.5);


            var compMat3 = new Printing3DCompositeMaterial();
            // fraction adds to 1.0
            compMat3.Values.Add(0.8);
            compMat3.Values.Add(0.2);

            var compMat4 = new Printing3DCompositeMaterial();
            // fraction adds to 1.0
            compMat4.Values.Add(0.4);
            compMat4.Values.Add(0.6);

            // add composites to group
            compGroup.Composites.Add(compMat);
            compGroup.Composites.Add(compMat2);
            compGroup.Composites.Add(compMat3);
            compGroup.Composites.Add(compMat4);


            model.Material.CompositeGroups.Add(compGroup);


            // texture2Coord Group

            // create new texture material group with id 4
            var tex2CoordGroup = new Printing3DTexture2CoordMaterialGroup(4);

            // create texture material
            var tex2CoordMaterial = new Printing3DTexture2CoordMaterial();
            // setup u,v values
            tex2CoordMaterial.U = 0.3;
            tex2CoordMaterial.V = 0.8;
            tex2CoordGroup.Texture2Coords.Add(tex2CoordMaterial);
            var tex2CoordMaterial2 = new Printing3DTexture2CoordMaterial();
            tex2CoordMaterial2.U = 0.3;
            tex2CoordMaterial2.V = 0.8;
            tex2CoordGroup.Texture2Coords.Add(tex2CoordMaterial2);

            var tex2CoordMaterial3 = new Printing3DTexture2CoordMaterial();
            tex2CoordMaterial3.U = 0.5;
            tex2CoordMaterial3.V = 0.8;
            tex2CoordGroup.Texture2Coords.Add(tex2CoordMaterial3);


            var tex2CoordMaterial4 = new Printing3DTexture2CoordMaterial();
            tex2CoordMaterial4.U = 0.5;
            tex2CoordMaterial4.V = 0.5;
            tex2CoordGroup.Texture2Coords.Add(tex2CoordMaterial4);

            // add metadata about the texture so that u,v values can be used
            model.Metadata.Add("tex4", "/3D/Texture/msLogo.png");
            // add group to groups on the model's material
            model.Material.Texture2CoordGroups.Add(tex2CoordGroup);

            // create vertices on the mesh
            await this.GetVerticesAsync(mesh);

            // create triangles on the mesh
            await SetTriangleIndicesAsync(mesh);

            // create material indices
            await SetMaterialIndicesAsync(mesh);

            // add the mesh to the model
            model.Meshes.Add(mesh);

            // create new component
            Printing3DComponent component = new Printing3DComponent();

            // assign mesh to the component's mesh
            component.Mesh = mesh;

            // add component to the model
            // a model can have multiple components
            model.Components.Add(component);

            // create the world matrix
            var componentWithMatrix = new Printing3DComponentWithMatrix();
            // assuign component
            componentWithMatrix.Component = component;

            // set translation to be used to identity
            var identityMatrix = Matrix4x4.Identity;

            componentWithMatrix.Matrix = identityMatrix;

            // add component to the build.
            // build defines what is to be printed from within a Printing3DModel
            // there maybe meshes one does not wish to print
            // anything that needs to be printed is put into the build component
            model.Build.Components.Add(componentWithMatrix);

            // now that we have a model, we save the model into a package
            await localPackage.SaveModelToPackageAsync(model);

            // get the 3mf file as a stream
            var stream = await localPackage.SaveAsync();
            // get the model stream
            var modelStream = localPackage.ModelPart;

            // fix any textures in the model file
            localPackage.ModelPart = await FixTextureContentType(modelStream);
            // save package in preparation to submit for printing
            SavePackage(localPackage);

            // optional step to save the 3mf file to disk
            await saveStreamTo3MF(stream);

            return true;
        }

        /// <summary>
        /// Fixes issue in API where textures are not saved correctly
        /// </summary>
        /// <param name="modelStream">3dmodel.model data</param>
        /// <returns></returns>
        private async Task<IRandomAccessStream> FixTextureContentType(IRandomAccessStream modelStream)
        {
            XDocument xmldoc = XDocument.Load(modelStream.AsStreamForRead());

            var outputStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
            var writer = new Windows.Storage.Streams.DataWriter(outputStream);
            writer.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
            writer.ByteOrder = Windows.Storage.Streams.ByteOrder.LittleEndian;
            writer.WriteString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

            var text = xmldoc.ToString();
            // ensure that content type is set correctly
            // texture content can be either png or jpg
            var replacedText = text.Replace("contenttype=\"\"", "contenttype=\"image/png\"");
            writer.WriteString(replacedText);

            await writer.StoreAsync();
            await writer.FlushAsync();
            writer.DetachStream();
            return outputStream;
        }

        /// <summary>
        /// Mechanism to notify user
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void Task_Submitting(Print3DTask sender, object args)
        {
            // notify user if required
        }

        /// <summary>
        /// Mechanism to notify user
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void Task_Completed(Print3DTask sender, Print3DTaskCompletedEventArgs args)
        {
            // notify user if required
        }

        /// <summary>
        /// Mechanism to save a stream to a storageFile
        /// </summary>
        /// <param name="stream">input data</param>
        /// <param name="storageFile">storage file</param>
        /// <returns></returns>
        private async static Task<bool> saveStreamTo3MF(Windows.Storage.Streams.IRandomAccessStream stream, StorageFile storageFile = null)
        {

            // set back
            stream.Seek(0);
            using (var dataReader = new Windows.Storage.Streams.DataReader(stream))
            {

                var dataLoad = await dataReader.LoadAsync((uint)stream.Size);
                if (dataLoad > 0)
                {


                    var buffer = dataReader.ReadBuffer((uint)stream.Size);
                    if (storageFile != null)
                    {
                        // use provided storageFile
                        await Windows.Storage.FileIO.WriteBufferAsync(storageFile, buffer);
                    }
                    else
                    {
                        StorageFolder localFolder = ApplicationData.Current.LocalFolder;
                        var filePath = Path.Combine(localFolder.Path, "output.3mf");
                        var outputfile = await localFolder.CreateFileAsync("output.3mf", CreationCollisionOption.ReplaceExisting);
                        await Windows.Storage.FileIO.WriteBufferAsync(outputfile, buffer);
                    }
                }
            }

            return true;
        }

        /// <summary>
        /// Set materialindices on the mesh
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        private static async Task SetMaterialIndicesAsync(Printing3DMesh mesh)
        {
            Printing3DBufferDescription description;

            description.Format = Printing3DBufferFormat.Printing3DUInt;
            // 4 indices for material description
            description.Stride = 4;
            // 12 triangles with material
            mesh.IndexCount = 12;
            mesh.TriangleMaterialIndicesDescription = description;
            // create buffer for storing data
            mesh.CreateTriangleMaterialIndices(sizeof(UInt32) * 4 * 12);

            var stream = mesh.GetTriangleMaterialIndices().AsStream();
            {
                UInt32[] indices =
                {
                    // base material
                    1, 1, 0, 0,
                    1, 0, 0, 0,
                    // color material
                    2, 1, 1, 2,
                    2, 0, 0, 1,
                    2, 1, 0, 0,
                    2, 0, 0, 0,
                    2, 0, 1, 2,
                    2, 1, 0, 2,
                    2, 1, 2, 0,
                    2, 2, 1, 0,
                    // texture material                    
                    4, 0, 0, 0,
                    4, 0, 0, 0,
                };

                var vertexData = indices.SelectMany(v => BitConverter.GetBytes(v)).ToArray();
                var len = vertexData.Length;
                await stream.WriteAsync(vertexData, 0, vertexData.Length);
            }

        }

        /// <summary>
        /// Set the triangle indices on the mesh
        /// </summary>
        /// <param name="mesh">Printing3DMesh</param>
        /// <returns></returns>
        private static async Task SetTriangleIndicesAsync(Printing3DMesh mesh)
        {
            Printing3DBufferDescription description;

            description.Format = Printing3DBufferFormat.Printing3DUInt;
            // 3 vertex position indices
            description.Stride = 3;
            // 12 triangles in all in the cube
            mesh.IndexCount = 12;
            mesh.TriangleIndicesDescription = description;

            mesh.CreateTriangleIndices(sizeof(UInt32) * 3 * 12);

            var stream2 = mesh.GetTriangleIndices().AsStream();
            {
                UInt32[] indices =
                {
                    0, 1, 2,
                    1, 3, 2,
                    0, 1, 5,
                    0, 5, 4,
                    1, 3, 7,
                    1, 7, 5,
                    2, 3, 7,
                    2, 7, 6,
                    0, 2, 6,
                    0, 6, 4,
                    6, 7, 5,
                    4, 6, 5,
                };

                var vertexData = indices.SelectMany(v => BitConverter.GetBytes(v)).ToArray();
                var len = vertexData.Length;
                await stream2.WriteAsync(vertexData, 0, vertexData.Length);
            }

        }

        private async Task GetVerticesAsync(Printing3DMesh mesh)
        {
            Printing3DBufferDescription description;

            description.Format = Printing3DBufferFormat.Printing3DDouble;
            // 3 xyz values
            description.Stride = 3;
            // 8 vertices in all in the cube
            mesh.CreateVertexPositions(sizeof(double) * 3 * 8);
            mesh.VertexPositionsDescription = description;

            using (var stream = mesh.GetVertexPositions().AsStream())
            {
                double[] vertices =
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

                byte[] vertexData = vertices.SelectMany(v => BitConverter.GetBytes(v)).ToArray();

                await stream.WriteAsync(vertexData, 0, vertexData.Length);
            }
            // 8 vertices in the cube
            mesh.VertexCount = 8;
        }


    }
}
