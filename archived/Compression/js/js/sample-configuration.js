(function() {
  "use strict";
  var sampleTitle = "Data Compression / Decompression SDK Sample";
  var scenarios = [{
    url: "/html/object.html",
    title: "JavaScript object compression / decompression"
}, {
    url: "/html/stream.html",
    title: "Bulk stream compression / decompression"
}];
  WinJS.Namespace.define("SdkSample", {
    sampleTitle: sampleTitle,
    scenarios: new WinJS.Binding.List(scenarios)
});
})();