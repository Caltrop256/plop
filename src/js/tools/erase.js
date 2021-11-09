class EraseTool {
    pressed = false;
    mx = -1; my = -1;

    ondown({x,y}) {
        this.mx = x;
        this.my = y;
        this.pressed = true;
        this.applyPaint(this.mx, this.my);
    }

    onup() {
        this.pressed = false;
    }

    ontick({x, y}) {
        if(this.pressed) {
            line(this.mx, this.my, x, y, this.applyPaint);
            this.mx = x;
            this.my = y;
        }
    }

    applyPaint(mx, my) {
        return wasm.exports.eraseArea(mx, my, areaOfEffect);
        for(let y = -areaOfEffect; y <= areaOfEffect; ++y) {
            const py = my + y;
            if(py < 0) continue;
            else if(py >= canvas.height) break;

            for(let x = -areaOfEffect; x <= areaOfEffect; ++x) {
                const px = mx + x;
                if(px < 0) continue;
                else if(px >= canvas.width) break;

                const cellPtr = wasm.exports.getCell(px, py);
                if(!cellPtr) continue;
                if(new Uint32Array(wasm.exports.memory.buffer, cellPtr, 1)[0]) {
                    wasm.exports.freeCell(cellPtr);
                }
            }
        }
    }
}