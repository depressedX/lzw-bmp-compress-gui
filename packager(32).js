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
    ignore: [/cache/, /output/,/cppsrc/],
    overwrite:true,
    arch:'ia32'
}
packager(options, function done_callback(err, appPaths) {
    if (err) {
        console.log(err)
    }else {
        console.log(appPaths)
    }
})