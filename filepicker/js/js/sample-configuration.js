(function() {
  "use strict";
  var sampleTitle = "File picker JS sample";
  var scenarios = [{
    url: "/html/scenario1_SingleFile.html",
    title: "Pick a single photo"
}, {
    url: "/html/scenario2_MultiFile.html",
    title: "Pick multiple files"
}, {
    url: "/html/scenario3_SingleFolder.html",
    title: "Pick a folder"
}, {
    url: "/html/scenario4_SaveFile.html",
    title: "Save a file"
}, {
    url: "/html/scenario5_OpenCachedFile.html",
    title: "Open a cached file"
}, {
    url: "/html/scenario6_SaveCachedFile.html",
    title: "Update a cached file"
}];
  WinJS.Namespace.define("SdkSample", {
    sampleTitle: sampleTitle,
    scenarios: new WinJS.Binding.List(scenarios)
});
})();