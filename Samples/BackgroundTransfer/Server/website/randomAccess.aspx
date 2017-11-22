<%@ Page Language="C#" AutoEventWireup="true" Debug="true" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<script runat="server">

private static void GetRange(uint fullContentLengthInBytes, string rangeHeaderValue, out uint offset, out uint lengthInBytes)
{
    offset = 0;
    lengthInBytes = fullContentLengthInBytes;

    if (String.IsNullOrEmpty(rangeHeaderValue))
    {
        return;
    }

    Regex rx = new Regex(@"(bytes)\s*=\s*(?<startIndex>\d+)\s*-\s*(?<endIndex>\d+)?", RegexOptions.Compiled | RegexOptions.IgnoreCase);
    Match match = rx.Match(rangeHeaderValue);

    uint startIndex = 0;
    Capture startIndexCapture = match.Groups["startIndex"];
    if ((startIndexCapture == null) || (!UInt32.TryParse(startIndexCapture.Value, out startIndex)))
    {
        return;
    }

    if (startIndex < 0 || startIndex >= fullContentLengthInBytes)
    {
        throw new ArgumentException(
            "Requested range starting index is negative or goes beyond the file's length");
    }

    offset = startIndex;

    uint endIndex = 0;
    Capture endIndexCapture = match.Groups["endIndex"];
    if ((endIndexCapture != null) && (UInt32.TryParse(endIndexCapture.Value, out endIndex)))
    {
        if (endIndex < startIndex)
        {
            throw new ArgumentException(
                "Requested range ending index is less than the starting index");
        }

        lengthInBytes = Math.Min(endIndex, fullContentLengthInBytes - 1) - offset + 1;
    }
    else
    {
        lengthInBytes = fullContentLengthInBytes - offset;
    }
}

protected void Page_Load(object sender, EventArgs e)
{
    try
    {
        uint fullContentLengthInBytes = 100000000; // 100 MB
        if (Request["length"] != null)
        {
            fullContentLengthInBytes = UInt32.Parse(Request["length"]);
        }

        int bytesPerSec = 1000000; // 1 MB/s
        if (Request["bytesPerSec"] != null)
        {
            bytesPerSec = Int32.Parse(Request["bytesPerSec"]);
            if (bytesPerSec <= 0)
            {
                throw new ArgumentException("bytesPerSec cannot be zero or negative");
            }
        }

        uint offset = 0;
        uint contentLengthInBytes = fullContentLengthInBytes;

        // Check if we have a request for partial content (resume scenarios).
        if (Request.Headers["Range"] != null)
        {
            GetRange(fullContentLengthInBytes, Request.Headers["Range"], out offset, out contentLengthInBytes);
        }

        if (contentLengthInBytes != fullContentLengthInBytes)
        {
            Response.StatusCode = 206;
            Response.Headers["Content-Range"] = String.Format(System.Globalization.CultureInfo.InvariantCulture,
                "bytes {0}-{1}/{2}", offset, offset + contentLengthInBytes - 1, fullContentLengthInBytes);
        }
        else
        {
            Response.StatusCode = 200;
        }

        Response.ContentType = "text/plain";
        Response.Headers["Content-Length"] = contentLengthInBytes.ToString(System.Globalization.CultureInfo.InvariantCulture);
        Response.Headers["Accept-Ranges"] = "bytes";
        Response.Headers["Last-Modified"] = "Thu, 21 Aug 2014 21:34:57 GMT";
        Response.Buffer = false;

        uint transferLengthInBytes = contentLengthInBytes;
        byte[] buffer = Encoding.ASCII.GetBytes(new String('a', bytesPerSec));
        while (transferLengthInBytes > 0)
        {
            System.Threading.Thread.Sleep(1000);

            int sendLengthInBytes = (int)Math.Min(transferLengthInBytes, buffer.Length);
            Response.OutputStream.Write(buffer, 0, sendLengthInBytes);
            transferLengthInBytes -= (uint)sendLengthInBytes;
        }

        Response.Flush();
    }
    catch (Exception ex)
    {
        Trace.Write(ex.Message);
        Response.StatusCode = 500;
        Response.StatusDescription = ex.Message;
    }

    Response.End();
}

</script>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Random Access Download</title>
</head>
<body>
    Hello
</body>
</html>
