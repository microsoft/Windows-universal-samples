using System;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Web.Http;

namespace LiveDash.Util
{

    public class Downloader
    {
        private const int fileNotFound404Hresult = -2145844844;
        public static int RETRIES = 3;
        public async static System.Threading.Tasks.Task<IBuffer> DownloadBufferAsync(Uri url)
        {
            //Turn off cache for live playback
            var filter = new Windows.Web.Http.Filters.HttpBaseProtocolFilter();
            filter.CacheControl.WriteBehavior = Windows.Web.Http.Filters.HttpCacheWriteBehavior.NoCache;
            using (var httpClient = new HttpClient(filter))
            {
                try
                {
                    var buffer = await httpClient.GetBufferAsync(url);
#if DEBUG
                    Logger.Log("GetBufferAsync suceeded for url: " + url);
#endif
                    return buffer;
                }
                catch (Exception e)
                {
#if DEBUG
                    Logger.Log("GetBufferAsync exception for url: " + url + " HRESULT 0x" + e.HResult.ToString("x"));
#endif
                    return null;
                }
            }
        }

        public async static Task<HttpResponseMessage> SendHeadRequestAsync(Uri url)
        {
            var filter = new Windows.Web.Http.Filters.HttpBaseProtocolFilter();
            filter.CacheControl.WriteBehavior = Windows.Web.Http.Filters.HttpCacheWriteBehavior.NoCache;
            using (var httpClient = new HttpClient(filter))
            {
                try
                {
                    HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Head, url);

                    HttpResponseMessage response = await httpClient.SendRequestAsync(request);
#if DEBUG
                    Logger.Log("Sending head request with: " + url);
#endif
                    return response;
                }
                catch (Exception e)
                {
#if DEBUG
                    Logger.Log("GetBufferAsync exception for url: " + url + " HRESULT 0x" + e.HResult.ToString("x"));
#endif
                    return null;
                }
            }
        }


    }
#if DEBUG
    public class Log
    {
        string CurrentLog = "";
        public StorageFile sampleFile;

        public Log()
        {
            CreateFile();
        }

        private async void CreateFile()
        {
            StorageFolder folder = ApplicationData.Current.LocalFolder;
            sampleFile = await folder.CreateFileAsync("LiveDashLog.txt", CreationCollisionOption.ReplaceExisting);
        }
        

        public async void WriteToFile(string s)
        {
            try
            {
                await FileIO.AppendTextAsync(sampleFile, s);
            }
            catch (Exception)
            {
                
            }
        }

        public string GetLog()
        {
            lock (this)
            {
                return CurrentLog;
            } 

        }

        public void Append(string s)
        {
            CurrentLog += s;
            if (CurrentLog.Length > 10000)
            {
                CurrentLog = CurrentLog.Substring(CurrentLog.Length - 10000);
            }
        }

        public void Clear()
        {
            lock (this)
            {
                CurrentLog = "";
            }
        }

    }
    

    public class Logger
    {
        public static Log logger = new Log();

        public static void Log(string s)
        {
            logger.Append(s + "\r\n");
            logger.WriteToFile(s + "\r\n");
        }

        public static StorageFile GetFileLog()
        {
            return logger.sampleFile;
        }

        public static string GetLog()
        {
            return logger.GetLog();
        }

        public static string Display(Exception e)
        {
            return "HRESULT: " + e.HResult.ToString("X8");
        }
    }
#endif
}
