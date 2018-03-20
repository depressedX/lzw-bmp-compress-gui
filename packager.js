const packager = require('electron-packager'),
    path = require('path')
const options = {
    dir:path.resolve('./'),
    download:{
        cache:path.resolve('./cache'),
        mirror:'https://npm.taobao.org/mirrors/electron/'
    },
    electronVersion:'1.8.4',
    out:path.resolve('./app'),
    ignore:/cache/,
    overwrite:true
}
packager(options, function done_callback(err, appPaths) { /* … */
})