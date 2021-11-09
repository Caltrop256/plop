class Element {
    static getElementIndexById(id) {
        for(let i = 0; i < renderList.length; ++i) {
            if(renderList[i].id == id) return i;
        }
        return null;
    }
    left; right; top; bottom;

    id = "unknown"

    get width() {
        return this.right - this.left;
    }

    get height() {
        return this.bottom - this.top;
    }

    get visible() {
        return this._visible;
    }

    set visible(b) {
        return this._visible = !!b;
    }

    constructor(pivot) {
        this.pivot = Vec2.from(pivot);
        this._visible = true;
    }

    pointInBoundary(x,y) {
        return (x > this.left && x < this.right) && (y > this.top && y < this.bottom);
    }

    on(name, func) {
        this['on' + name] = func;
    }
}

let frameExplosionPower = 0;

(() => {
    const uiCanvas = document.getElementById('ui');
    const ctx = uiCanvas.getContext('2d');

    window.renderList = [];

    setInterval(() => {
        if(typeof wasm == 'undefined' || typeof infoNode == 'undefined') return;
        let temperature = '0';
        let tile = 'VOID';
        const {x,y} = eventhandler.getCanvasCoords();
        if(eventhandler.inCanvas()) {
            temperature = wasm.exports.getTemp(x, y).toFixed(1);
            tile = enumToString[wasm.exports.getType(wasm.exports.getCell(x, y))];
        }
        const newInfostring = `${x}x, ${canvas.height - y}y, ${temperature}°C, ${tile}`;
        infoNode.changeText(newInfostring);

        if(frameExplosionPower < 1) {
            frameExplosionPower = 0;
            TextNode.shake = 0;
            Point.roughness = 0;
        } else {
            TextNode.shake = Math.min(25, frameExplosionPower / 10);
            Point.roughness = 10;
            frameExplosionPower *= 0.8;
        }

        Point.roughness = Math.min(3, Math.max(0, temperature / 250 + 0.48)) + frameExplosionPower / 1000;

        ctx.clearRect(0,0,uiCanvas.width,uiCanvas.height);
        for(const drawable of renderList) if(drawable.visible) drawable.draw(ctx);

        const radiationCoeff = Math.min(0.7, wasm.exports.getNSubatomics() / 15000);
        if(!radiationCoeff) return;

        const imageData = ctx.getImageData(0, 0, uiCanvas.width, uiCanvas.height);

        for(let i = 3; i < imageData.data.length; i += 4) {
            if(imageData.data[i] && Math.random() < radiationCoeff) imageData.data[i] = 0;
        }
        ctx.putImageData(imageData, 0, 0);
    }, 50);

    window.addEventListener('resize', () => {
        uiCanvas.width = window.innerWidth;
        uiCanvas.height = window.innerHeight;
        ctx.strokeStyle = '#ffffff';
        renderList.length = 0;
        window.constructUI(renderList)
    });
    window.addEventListener('load', () => {
        uiCanvas.width = window.innerWidth;
        uiCanvas.height = window.innerHeight;
        ctx.strokeStyle = '#ffffff';
        renderList.length = 0;
    });
})();

function line(x0, y0, x1, y1, callback) {
    const dx = Math.abs(x1 - x0);
    const dy = -Math.abs(y1 - y0);
    const sx = x0 < x1 ? 1 : -1;
    const sy = y0 < y1 ? 1 : -1;

    let err = dx + dy;

    for(;;) {
        callback(x0, y0);
        if(x0 == x1 && y0 == y1) return;
        const e2 = 2 * err;
        if(e2 >= dy) {
            err += dy;
            x0 += sx;
        };
        if(e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}