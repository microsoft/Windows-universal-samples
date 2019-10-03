// This creates a single public function, Windows.Globalization.preferredFonts(language), which returns an array of fonts
// whose members are named the same as their equivalent in DOM Style elements (fontFamily, fontStyle, fontStretch,
// fontWeight, fontSize).  The array returned has the following members:
//        Win.UI.preferredFonts(someLanguage).uiTextFont,         // font for UI text
//        Win.UI.preferredFonts(someLanguage).uiHeadingFont,      // font for UI headings
//        Win.UI.preferredFonts(someLanguage).uiCaptionFont,      // font for UI captions
//        Win.UI.preferredFonts(someLanguage).documentTextFont,   // font for document text
//        Win.UI.preferredFonts(someLanguage).documentHeadingFont,// font for document headings
//        Win.UI.preferredFonts(someLanguage).messagingTextFont,  // font for messaging text (emails, IMs, etc)
//        Win.UI.preferredFonts(someLanguage).fixedWidthTextFont, // font for fixed width
// Each of these recommended fonts has the following members (using uiTextFont as an example)…
//        Win.UI.preferredFonts(someLanguage).uiTextFont.fontFamily
//        Win.UI.preferredFonts(someLanguage).uiTextFont.fontStyle
//        Win.UI.preferredFonts(someLanguage).uiTextFont.fontStretch
//        Win.UI.preferredFonts(someLanguage).uiTextFont.fontWeight
//        Win.UI.preferredFonts(someLanguage).uiTextFont.fontSize
//
//  This wrapper around the Win8 RT preferred fonts API is used to map the ABI native names/types to the strings that
//  are expected in CSS/js.  It is intended that this functionality is provided in a toolkit, rather than have each
// client write their own.
(function () {

    // translation strings (translate enum values from WinRT, FontStyle and FontStretch, to CSS/DOM strings)
    var stretches = [
    "normal", // Default
    "ultra-condensed",
    "extra-condensed",
    "condensed",
    "semi-condensed",
    "normal",
    "semi-expanded",
    "expanded",
    "extra-expanded",
    "ultra-expanded"
    ];
    var styles = ["normal", "italic", "oblique"];

    // This function returns a font object whose members are named the same as DOM Style elements.
    // The input is a Windows.Globalization.Text.preferredFont.
    function convertRecommendedFont(recFont) {

        var font = {
                fontFamily: recFont.fontFamily,
                fontStyle: (recFont.fontStyle !== undefined) ? styles[recFont.fontStyle] : null,
                fontStretch: (recFont.fontStretch !== undefined) ? stretches[recFont.fontStretch] : null,
                fontWeight: (recFont.fontWeight !== undefined) ? recFont.fontWeight.weight : null,
                scaleFactor: recFont.scaleFactor
            };

            // CSS doesn't support semilight weight value 350.
            // Provide a workaround for Segoe UI Semilight (used in Windows 8.1 style guidelines for UI text
            // in Latin and other scripts)
            if (font.fontFamily === "Segoe UI" && font.fontWeight === 350) {
                font.fontFamily = "Segoe UI Semilight";
                font.fontWeight = 400;
            }

        return font;
    }

    //  This public function returns an array of recommended fonts for a particular language.
    WinJS.Namespace.define("WinJS.UI", {
        preferredFonts: function (language) {

            var fonts = new Windows.Globalization.Fonts.LanguageFontGroup(language);

            var result = {};

            for (var category in fonts) {
                try {
                    result[category] = convertRecommendedFont(fonts[category]);
                } catch (e) { }
            }

            return result;
        },

        applyFontToElement: function (el, font) {
            for (var i in font) {
                if (font[i] && el.style[i] !== undefined) {
                    el.style[i] = font[i];
                }
            }
        }
    });
})();
