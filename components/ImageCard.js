let exec = require('child_process').exec,
    path = require('path'),
    fs = require('fs'),
    os = require('os'),
    tmpdir = os.tmpdir()
var iconv = require('iconv-lite');
var encoding = 'cp936';
var binaryEncoding = 'binary';
const shell = require('electron').shell

module.exports = {
    template: `
<el-card 
    class="image-card" 
    @dragover.native.prevent="" 
    style="display: inline-block;margin: 5px">
    <div class="image-container"
        ref="imageContainer"
        @dragenter="handleDragenter"
        @dragleave="handleDragleave"
        @drop.prevent="handleDrop">
        <div class="image-default" @dragleave.stop="" v-if="state===IDLE">
            <div class="image-default-hint-text">
                <i class="el-icon-circle-plus-outline"></i>
                <p style="margin: .2em 0">拖拽到这里</p>
            </div>
        </div>
        <div class="image-hover" @dragleave.stop="" v-else-if="state===IMAGE_HOVERING">
            <div class="image-hover-hint-text">
                <i class="el-icon-download"></i>
                <p style="margin: .2em 0">释放鼠标</p>
            </div>
        </div>
        <div class="image-converting" @dragleave.stop="" v-else-if="state===CONVERTING">
            <div class="image-hover-hint-text">
                <i class="el-icon-loading"></i>
                <p style="margin: .2em 0">正在处理</p>
            </div>
        </div>
        <img :src="imgSrc" @dragleave.stop="" v-else>
    </div>
    <div style="padding: 14px 0 0 0;">
        <el-button 
        type="success" 
        icon="el-icon-circle-plus-outline" 
        size="mini" 
        v-if="state===IDLE"
        @click="$refs.importInput.click()"
        >从文件导入</el-button>
        <el-button 
        type="danger" 
        icon="el-icon-picture" 
        size="mini" 
        v-else-if="state===DISPLAYING&&operationType===ZIP"
        @click="exportFile('.lzw')"
        >导出*.lzw</el-button>
        <el-button 
        type="primary" 
        icon="el-icon-picture-outline" 
        size="mini" 
        v-else
        @click="exportFile('.bmp')"
        >导出*.bmp</el-button>
        <input type="file" hidden ref="importInput" @change="importInputHandler">
    </div>
</el-card>
    `,
    props: {
        outputDir: {
            type: String,
            default: path.resolve('./output')
        },
        // 标记在父组件中的位置 用于删除等操作
        index: {
            default: Math.random()
        }
    },
    data() {
        return {
            imgSrc: null,

            state: 0,
            IDLE: 0,//还没有进行才做
            IMAGE_HOVERING: 1,//拖拽悬停
            CONVERTING: 3,//正在处理图片
            DISPLAYING: 2,//正在展示图片
            DONE: 4,


            operationType: 0,
            ZIP: 0,
            UNZIP: 1,

            tmpFilePath: null
        }
    },
    methods: {
        zipBmp(filePath) {
            if (this.state !== this.IDLE) return
            this.state = this.CONVERTING
            let filename = Date.now() + '.lzw'
            let tmpFilePath = path.join(tmpdir, filename)
            exec(`${path.join(__dirname, '../lzw.exe')} zip ${filePath} ${tmpFilePath}`, {encoding: binaryEncoding}, (err, stdout, stderr) => {
                if (err) {
                    console.log(iconv.decode(new Buffer(stdout, binaryEncoding), encoding), iconv.decode(new Buffer(stderr, binaryEncoding), encoding));
                } else {
                    this.imgSrc = filePath
                    this.state = this.DISPLAYING
                    this.operationType = this.ZIP
                    this.tmpFilePath = tmpFilePath
                    this.$message('成功')
                }
            });
        },
        unzipLzw(filePath) {
            if (this.state !== this.IDLE) return
            this.state = this.CONVERTING
            let filename = Date.now() + '.bmp'
            let tmpFilePath = path.join(tmpdir, filename)
            exec(`${path.join(__dirname, '../lzw.exe')} unzip ${filePath} ${tmpFilePath}`, {encoding: binaryEncoding}, (err, stdout, stderr) => {
                if (err) {
                    console.log(iconv.decode(new Buffer(stdout, binaryEncoding), encoding), iconv.decode(new Buffer(stderr, binaryEncoding), encoding));
                } else {
                    this.imgSrc = tmpFilePath
                    this.state = this.DISPLAYING
                    this.operationType = this.UNZIP
                    this.tmpFilePath = tmpFilePath
                    this.$message('成功')
                }
            });
        },
        exportFile(ext) {
            if (!(this.state === this.DISPLAYING)) return
            try {
                fs.mkdirSync(this.outputDir)
            } catch (e) {
            }
            let outputFile = path.join(this.outputDir, `${Date.now()}${ext}`)
            fs.rename(this.tmpFilePath, outputFile, (err) => {
                if (err) throw err;
                this.$message('导出成功')
                this.state = this.DONE
            });
        },
        importInputHandler(e) {
            let file = e.target.files[0]
            if (!file) return
            if (/\.(lzw)$/i.test(file.path)) {
                this.unzipLzw(file.path)
            } else if (/\.(bmp)$/i.test(file.path)) {
                this.zipBmp(file.path)
            } else {
                this.$message('文件类型匹配失败:必须是*.lzw/*.bmp')
            }
        },
        handleDrop(e) {
            if (!e.dataTransfer.files) return
            let file = e.dataTransfer.files[0]
            if (!file) return
            if (/\.(lzw)$/i.test(file.path)) {
                this.state = this.IDLE
                this.unzipLzw(file.path)
            } else if (/\.(bmp)$/i.test(file.path)) {
                this.state = this.IDLE
                this.zipBmp(file.path)
            } else {
                this.$message('文件类型匹配失败:必须是*.lzw/*.bmp')
            }
        },
        handleDragenter(e) {
            if (!(this.state === this.IDLE)) return
            this.state = this.IMAGE_HOVERING
        },
        handleDragleave(e) {
            if (e.offsetX <= e.target.clientWidth && e.offsetX>=0 &&e.offsetY <= e.target.clientHeight && e.offsetY>=0 ) return
            if (!(this.state === this.IMAGE_HOVERING)) return
            this.state = this.IDLE
        },
    },
    watch: {
        state(val) {
            this.$emit('statechange', this, val)
        }
    }
}