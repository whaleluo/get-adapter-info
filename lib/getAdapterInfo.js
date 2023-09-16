let addon = null
function getAdapterInfo() {
    if(process.platform !== 'win32'){
        return null
    }
    addon = require('../build/Release/getAdapterInfo.node');
    return addon()
}
module.exports = getAdapterInfo