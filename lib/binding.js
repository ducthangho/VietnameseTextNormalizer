const addon = require('../build/Release/vietnamesetextnormalizer');

function VietnameseTextNormalizer() {
    this.Normalize = function(str) {
        return _addonInstance.Normalize(str);
    }

    this.NormalizeForTTS = function(str) {
        return _addonInstance.NormalizeForTTS(str);
    }

    var _addonInstance = new addon.VietnameseTextNormalizer();
}

module.exports = VietnameseTextNormalizer;
