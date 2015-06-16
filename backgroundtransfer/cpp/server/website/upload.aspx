<%@ Page Language="C#" AutoEventWireup="true" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<script runat="server">

protected void Page_Load(object sender, EventArgs e)
{
    try
    {
        if (Request.Headers["Filename"] != null)
        {
            // Simple upload scenario.
            string fileName = Request.Headers["Filename"];
            Response.Write("Filename is " + fileName);

            string saveLocation = Server.MapPath("Data") + "\\" + fileName;
            using (System.IO.FileStream fs = new System.IO.FileStream(saveLocation, System.IO.FileMode.Create))
            {
                Request.InputStream.CopyTo(fs);
            }
        }
        else
        {
            string formData = Request.Form["FormData"];
            for (int i = 0; i < Request.Files.Count; i++)
            {
                var file = Request.Files[i];
                if (file.ContentLength > 0)
                {
                    string fileName = System.IO.Path.GetFileName(file.FileName);
                    string saveLocation = Server.MapPath("Data") + "\\" + fileName;
                    file.SaveAs(saveLocation);
                }
            }
        }
    }
    catch (Exception ex)
    {
        Trace.Write(ex.Message);
        Response.StatusCode = 500;
        Response.StatusDescription = ex.Message;
        Response.End();
    }
}

</script>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Upload</title>
</head>
<body>
    Hello
</body>
</html>
