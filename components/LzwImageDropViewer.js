module.exports = {
    template: `<div
class="lzw-image-drop-viewer"
@dragover.stop.prevent=""
@drop.stop.prevent="dropHandler">
<div v-if="!imageSrc">将文件拖动到这里</div>
<img :src="imageSrc" v-if="imageSrc"/>
</div>`,
    data() {
        return {
            displaying: false,
            imageSrc: null
        }
    },
    methods: {
        dropHandler(e) {
            var dt = e.dataTransfer
            var file = dt.files[0]
            console.log(file)
            let fileUrl = URL.createObjectURL(file)
            this.imageSrc = fileUrl
        }
    }
}