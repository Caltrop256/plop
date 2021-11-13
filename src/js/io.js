function exportData() {
    const ptr = wasm.exports.exportData();

    const [,,,cellLength, cellSize, cellArrayOffset] = new Uint32Array(wasm.exports.memory.buffer, ptr, 6);
    const byteLen = cellLength * cellSize + cellArrayOffset;

    const view = new Uint8Array(wasm.exports.memory.buffer, ptr, byteLen);
    const buffer = new Uint8Array(byteLen);

    for(let i = 0; i < byteLen; ++i) {
        buffer[i] = view[i];
    };
    wasm.exports.free(ptr);
    return pako.deflate(buffer);
}

function importData(compressed) {
    const buffer = pako.inflate(compressed);

    const byteLen = buffer.byteLength;
    const ptr = wasm.exports.malloc(byteLen);
    const view = new Uint8Array(wasm.exports.memory.buffer, ptr, byteLen);
    for(let i = 0; i < byteLen; ++i) {
        view[i] = buffer[i];
    }
    const success = wasm.exports.importData(ptr);
    wasm.exports.free(ptr);

    if(!success) {
        console.error('Invalid File!');
        return false;
    }

    const size = buffer[8];

    canvas.width = size * 75;
    canvas.height = size * 75;
    imageData = new ImageData(
        createView('Uint8Clamped', 'imageData', canvas.width * canvas.height * 4, true), 
        canvas.width, canvas.height
    );
    return true;
}

function submitState() {
    state = exportData();
    fetch('./plopstate.emb', {
        headers: {
            'Content-Type': 'application/octet-stream',
            'Content-Size': state.byteLength
        },
        method: 'POST',
        body: state.buffer
    }).then(res => {
        if(res.ok) {
            res.text().then(id => {
                history.pushState({}, '', '/plop/' + id + '/');
            })
        }
    })
}