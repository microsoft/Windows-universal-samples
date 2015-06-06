<%@ Page Language="C#" AutoEventWireup="true" Debug="true" %>
<%@ import namespace="System.Drawing" %>
<%@ import namespace="System.Drawing.Imaging" %>
<%@ import namespace="System.IO" %>
<script runat="server">

private static Random random = new Random();

protected void Page_Load(object sender, EventArgs e)
{
    try
    {
        // Introduce an artificial delay, to simulate the latency between requests and responses.
        // This way, it is easier to appreciate each execution of the completion group background task.
        System.Threading.Thread.Sleep(2000);

        // Randomly fail, so Scenario4_CompletionGroups can retry the failed downloads.
        if (RandomlyFail())
        {
            ServeFailure();
        }
        else
        {
            ServeBitmap();
        }
    }
    catch (Exception ex)
    {
        Trace.Write(ex.Message);
        Response.StatusCode = 500;
        Response.StatusDescription = ex.Message;
    }

    Response.End();
}

private bool RandomlyFail()
{
    if (random.Next(10) >= 5)
    {
        return true;
    }
    return false;
}

private void ServeFailure()
{
    Response.StatusCode = 400;
}

private void ServeBitmap()
{
    Response.ContentType = "image/png";

    CreateBitmap(Response.OutputStream);
}

private static SolidBrush GetRandomBrush()
{
    Color color = Color.FromArgb(random.Next(255), random.Next(255), random.Next(255));
    return new SolidBrush(color);
}

private static void CreateBitmap(Stream stream)
{
    Bitmap bitmap = new Bitmap(1000, 1000);

    using (Graphics graphics = Graphics.FromImage(bitmap))
    {
        using (SolidBrush brush = GetRandomBrush())
        {
            graphics.FillRectangle(brush, 0, 0, bitmap.Width, bitmap.Height);
        }

        using (SolidBrush brush = GetRandomBrush())
        {
            graphics.FillEllipse(brush, 0, 0, bitmap.Width, bitmap.Height);
        }
    }

    bitmap.Save(stream, ImageFormat.Png);
}

</script>
