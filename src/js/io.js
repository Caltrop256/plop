function exportData() {
    const byteLen = wasm.exports.exportByteLen();
    const buffer = new Uint8Array(byteLen);
    const ptr = wasm.exports.exportData();
    const view = new Uint8Array(wasm.exports.memory.buffer, ptr, byteLen);
    for(let i = 0; i < byteLen; ++i) {
        buffer[i] = view[i];
    };
    wasm.exports.free(ptr);
    return buffer;
}

function importData(buffer) {
    const [magic, width, height] = new Uint16Array(buffer.buffer, 0, 3);
    if(magic != 0xB00B) throw new TypeError('Invalid File');

    const byteLen = buffer.byteLength;
    const ptr = wasm.exports.malloc(byteLen);
    const view = new Uint8Array(wasm.exports.memory.buffer, ptr, byteLen);
    for(let i = 0; i < byteLen; ++i) {
        view[i] = buffer[i];
    }
    wasm.exports.importData(ptr);
    wasm.exports.free(ptr);

    canvas.width = width;
    canvas.height = height;
    imageData = new ImageData(
        createView('Uint8Clamped', 'imageData', canvas.width * canvas.height * 4, true), 
        canvas.width, canvas.height
    );
}