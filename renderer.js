let ImageCard = require('./components/ImageCard')
const fs = require('fs')
let path = require('path')
const shell = require('electron').shell

new Vue({
    el: '#app',
    components: {
        ImageCard
    },
    template: `
<el-card class="box-card">
    <div slot="header" class="clearfix"> 
        <input type="file" hidden ref="fileInput" @change="importInputHandler">
        <el-button type="danger" icon="el-icon-picture" @click="$refs.fileInput.click()">压缩bmp</el-button>
        <el-button type="primary" icon="el-icon-picture-outline" @click="$refs.fileInput.click()">解压bmp</el-button> 
        <el-button type="text" style="float: right;" @click="openOutputDir">打开输出文件夹</el-button>
    </div>
    <div>
        <image-card ref="imageCards" :key="key" :keyProp="key" v-for="key in displayingImageCards" :outputDir="outputDir" @statechange="handleImageCardStateChange"/>
    </div>
</el-card>
    `,
    data() {
        return {
            outputDir: path.resolve('./output'),
            displayingImageCardNum: 0,
            displayingImageCards: [this.generateKey()],
        }
    },
    mounted() {
    },
    methods: {
        openOutputDir() {
            shell.showItemInFolder(path.join(this.outputDir, './'))
        },
        handleImageCardStateChange(context, state) {
            if (state === context.DISPLAYING) {
                // this.displayingImageCardNum++
                this.displayingImageCards.push(this.generateKey())
            } else if (state === context.DONE) {
                this.displayingImageCards.splice(context.index, 1)
            }
        },
        importInputHandler(e) {
            // 检查是否选中文件
            let files = e.target.files
            if (!files[0]) return
            this.$refs.imageCards[this.$refs.imageCards.length - 1].importInputHandler(e)
            e.target.value = ''
        },
        generateKey() {
            return Math.random()
        }
    },
    computed: {}
})