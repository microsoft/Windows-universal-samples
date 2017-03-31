namespace SDKTemplate
{
    public class FileSystemData
    {
        public FileSystemData(string name)
        {
            this.Name = name;
        }

        public string Name { get; set; }

        public bool IsFolder { get; set; }
    }
}
