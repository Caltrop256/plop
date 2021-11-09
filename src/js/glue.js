const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

let wasm;
let __memoryLen = 0;
let imageData;

let areaOfEffect = 3;
let elementInBrush;
let paused = false;
let categorySelected = 'brittle';

function setSize(n) {
    canvas.width = 75 * n;
    canvas.height = 75 * n;
    wasm.exports.setSize(canvas.width, canvas.height);
    const view = createView('Uint8Clamped', 'imageData', canvas.width * canvas.height * 4, true);
    imageData = new ImageData(view, canvas.width, canvas.height);
}

const toolList = [
    {
        name: 'draw',
        symbol: '\u0008',
        constructor: PaintTool
    }, {
        name: 'erase',
        symbol: '\u0000',
        constructor: EraseTool
    }, {
        name: 'line',
        symbol: '\u0009',
        constructor: LineTool
    }, {
        name: 'wind',
        symbol: '\u0010',
        constructor: WindTool
    }
];
const tools = [new PaintTool, new EraseTool];
const toolColors = ['#ffd9b5', '#b5f1ff'];

let resizeMenuOpen = false;
const controls = [
    {
        name: 'pause',
        symbol: '\u0011',
        callback(node, text){
            paused = !paused;
            node.changeText(paused ? '\u0012' : '\u0011');
            text.changeText(!paused ? 'PAUSE' : 'RESUME');
        }
    }, {
        name: 'step',
        symbol: '\u0015',
        callback() {
            wasm.exports.tick();
        }
    }, {
        name: 'reset',
        symbol: '\u0013',
        callback() {
            wasm.exports.changeScene();
        }
    }, {
        name: 'resize',
        symbol: '\u0014',
        callback() {
            if(resizeMenuOpen) {
                renderList[Element.getElementIndexById('resizeMenuCancel')].onmousedown();
            } else {
                resizeMenuOpen = true;
                const {top,left,right,bottom} = canvas.getBoundingClientRect();
                const scaleF = window.innerWidth < 620 ? range(window.innerWidth / 620, 0.1, 1.5) : 2;
                const description = new TextNode('Resize canvas', 1.8 * scaleF, new Vec2(left + (right - left) / 2, top + (bottom - top) / 2 - 60 * scaleF), 'center');
                renderList.push(description);

                let buttonRowWidth = 0;
                const sizes = [1, 2, 4, 6, 8, 10, 15, 20];
                for(let i = 0; i < sizes.length; ++i) {
                    buttonRowWidth += new Button(sizes[i] * 75 + '²', 1 * scaleF, new Vec2()).width + 4 * scaleF;
                }
                let resButtonX = 0;
                const buttons = [];
                for(let i = 0; i < sizes.length; ++i) {
                    const val = sizes[i];
                    const resButton = new Button(sizes[i] * 75 + '²', 1 * scaleF, new Vec2(left + (right - left) / 2 + resButtonX - buttonRowWidth / 2, top + (bottom - top) / 2 - 20 * scaleF));
                    resButton.on('mouseenter', () => {
                        document.body.style.cursor = 'pointer';
                    });
                    resButton.on('mouseexit', () => {
                        document.body.style.cursor = 'default'
                    });
                    resButton.on('mousedown', () => {
                        setSize(val);
                        renderList[Element.getElementIndexById('resizeMenuCancel')].onmousedown();
                    });
                    resButtonX += resButton.width + 4 * scaleF;
                    renderList.push(resButton);
                    buttons.push(resButton);
                }
                const cancelWidth = new Button('cancel', 1 * scaleF, new Vec2()).width;
                const cancel = new Button('cancel', 1 * scaleF, new Vec2(left + (right - left) / 2 - cancelWidth / 2, top + (bottom - top) / 2 + 5 * scaleF));
                cancel.id = 'resizeMenuCancel';
                cancel.on('mouseenter', () => {
                    document.body.style.cursor = 'pointer';
                });
                cancel.on('mouseexit', () => {
                    document.body.style.cursor = 'default'
                });
                cancel.on('mousedown', () => {
                    resizeMenuOpen = false;
                    renderList.splice(renderList.indexOf(cancel), 1);
                    renderList.splice(renderList.indexOf(description), 1);
                    for(const button of buttons) {
                        renderList.splice(renderList.indexOf(button), 1);
                    }
                })
                renderList.push(cancel);
            }
        }
    }
]

const eventhandler = new EventHandler();

let infoNode;

window.constructUI = (renderList) => {
    resizeMenuOpen = false;
    let lastSelected = lookup[enumToString[elementInBrush]].name || enumToString[elementInBrush];
    const {top,left,right,bottom} = canvas.getBoundingClientRect();

    const scaleF = Math.min(window.innerWidth) < 620 ? range(Math.min(window.innerWidth) / 620, 0.1, 1.5) : 2;
    let catX = 8;
    for(const category in categories) {
        if(category == 'pseudo' || category == 'hidden') continue;
        const node = new TextNode(catSymbols[categories[category]], scaleF, new Vec2(left + catX, top - 13 * scaleF));
        node.id = 'cat_' + category;

        const buttons = [];

        let by = 0;
        let furthestRight = 0;
        let furthestBottom = 0;
        for(const element in lookup) {
            const el = lookup[element];
            if(el.category == categories[category]) {
                let _x = catX;
                const button = new Button(el.name || element, scaleF, new Vec2(left + catX - 2 * scaleF, top + by));
                if((el.name || element) == lastSelected) button.clr = toolColors[0];
                button.id = 'elementselect_' + (el.name || element);
                button.visible = false;
                let cby = by;
                button.on('mousedown', () => {
                    const ind = Element.getElementIndexById('elementselect_' + lastSelected);
                    if(ind != null) delete renderList[ind].clr;
                    const catInd = Element.getElementIndexById('cat_' + categorySelected);
                    if(catInd != null) delete renderList[catInd].clr;
                    elementInBrush = el.id;
                    button.clr = toolColors[0];
                    lastSelected = el.name || element;
                    categorySelected = category;
                    node.clr = toolColors[0];
                    renderList[Element.getElementIndexById('bounding_' + category)].onmouseexit();
                    generateFavicon(catSymbols[categories[category]]);
                    button.onmouseexit();
                });
                button.on('mouseenter', () => {
                    document.body.style.cursor = 'pointer';
                    if(eventhandler.isPhone) return;
                    const desc = new TextNode(lookup[element].description, scaleF * 0.8, new Vec2(button.width + left + _x + 8 * scaleF, top + cby + 5 * scaleF));
                    desc.id = 'description_' + element;
                    renderList.push(desc);
                });
                button.on('mouseexit', () => {
                    document.body.style.cursor = 'default';
                    if(!eventhandler.isPhone) renderList.splice(Element.getElementIndexById('description_' + element));
                });

                if(button.bottom > furthestBottom) furthestBottom = button.bottom;
                if(button.right > furthestRight) furthestRight = button.right;

                by += button.height + 4 * scaleF;
                renderList.push(button);
                buttons.push(button);
            }
        }

        node.on('mouseenter', () => {
            if(Element.getElementIndexById('bounding_' + category)) return;
            for(const _cat in categories) {
                const bId = Element.getElementIndexById('bounding_' + _cat);
                if(bId != null) {
                    renderList[bId].onmouseexit();
                }
            }
            for(const button of buttons) button.visible = true;
            const name = new TextNode(category, scaleF, new Vec2(left + catX + 16 * scaleF, top - 13 * scaleF));
            renderList.push(name);

            const boundingBox = new Element(new Vec2());
            boundingBox.draw = () => {};
            boundingBox.id = 'bounding_' + category;
            boundingBox.top = -100;
            boundingBox.left = left - 8;
            boundingBox.right = furthestRight + 8;
            boundingBox.bottom = furthestBottom + 8;

            boundingBox.on('mouseexit', () => {
                const id = Element.getElementIndexById('bounding_' + category);
                if(id == null) return;
                for(const button of buttons) button.visible = false;
                renderList.splice(id, 1);
                renderList.splice(renderList.indexOf(name), 1);
            });
            renderList.push(boundingBox);
        });


        catX += node.width + 4 * scaleF;
        renderList.push(node);
    }

    let toolSizeDrawable;
    const toolSizeText = new TextNode('Tool Strength', 1.8, new Vec2(left + 4 * scaleF, top + 2 * scaleF * 1.8));
    toolSizeText.visible = false;
    const toolSize = new Element(new Vec2());
    toolSize.id = 'tool_areaOfEffect';
    toolSize.right = left - 2;
    toolSize.left = toolSize.right - scaleF * 18 - 5;
    toolSize.top = top;
    toolSize.bottom = top + scaleF * 18;
    toolSize.draw = (ctx) => {
        toolSizeDrawable.drawAt(ctx, new Vec2(left - 3 * scaleF - scaleF * 18, top));
    }
    toolSize.on('mouseenter', () => {
        document.body.style.cursor = 'pointer';
        toolSizeText.visible = true;
    });
    toolSize.on('mouseexit', () => {
        document.body.style.cursor = 'default'
        toolSizeText.visible = false;
    });
    toolSize.on('mousedown', e => {

        let size;
        if(e.button == 0) {
            switch(areaOfEffect) {
                case 0 : areaOfEffect = 3;   size = 0.7142857142857143; break;
                case 3 : areaOfEffect = 5;   size = 1.4285714285714286; break;
                case 5 : areaOfEffect = 7;   size = 2.142857142857143; break;
                case 7 : areaOfEffect = 10;  size = 2.857142857142857; break;
                case 10 : areaOfEffect = 15; size = 3.5714285714285716; break;
                case 15 : areaOfEffect = 30; size = 4.285714285714286; break;
                case 30 : areaOfEffect = 50; size = 5; break;
                case 50 : areaOfEffect = 0;  size = 0; break;
            }
        } else {
            switch(areaOfEffect) {
                case 0 : areaOfEffect = 50;  size = 5; break;
                case 3 : areaOfEffect = 0;   size = 0; break;
                case 5 : areaOfEffect = 3;   size = 0.7142857142857143; break;
                case 7 : areaOfEffect = 5;   size = 1.4285714285714286; break;
                case 10 : areaOfEffect = 7;  size = 2.142857142857143; break;
                case 15 : areaOfEffect = 10; size = 2.857142857142857; break;
                case 30 : areaOfEffect = 15; size = 3.5714285714285716; break;
                case 50 : areaOfEffect = 30; size = 4.285714285714286; break;
            }
        }

        toolSizeDrawable = new Drawable([
            0, 0,
            0, 10,
            10, 10,
            10, 0,

            5 + size, 5,
            5, 5 + size,
            5 - size, 5,
            5, 5 - size,

            5 + size, 5 + size,
            5 - size, 5 + size,
            5 - size, 5 - size,
            5 + size, 5 - size
        ], [
            [0, 1],
            [1, 2],
            [2, 3],
            [3, 0],

            [4, 5, 8],
            [5, 6, 9],
            [6, 7, 10],
            [7, 4, 11]
        ], scaleF * 1.8)
    });
    renderList.push(toolSize, toolSizeText);
    toolSize.onmousedown({button: 2});
    toolSize.onmousedown({button: 0});


    let toolY = 21 * scaleF * 2;
    let _i = 0;
    for(const tool of toolList) {
        const toolNode = new TextNode(tool.symbol, scaleF * 1.8, new Vec2(left - 2 * scaleF, top + toolY), 'right');
        tool.node = toolNode;
        let i = _i;

        for(let k = 0; k < 2; ++k) {
            if(tools[k] instanceof tool.constructor) {
                if(tools[k] instanceof LineTool) tools[k] = new LineTool();
                toolNode.clr = toolColors[k];
                tools[k].id = i;
            }
        }

        const name = new TextNode(tool.name, scaleF, new Vec2(left + 4 * scaleF, top + toolY + 3 * scaleF * 1.8));
        name.visible = false;
        renderList.push(name);

        toolNode.on('mouseenter', () => {
            document.body.style.cursor = 'pointer';
            name.visible = true;
        });
        toolNode.on('mouseexit', () => {
            document.body.style.cursor = 'default'
            name.visible = false;
        });
        toolNode.on('mousedown', e => {
            const button = e.button == 0 ? 0 : 1;
            const opposite = +!button; 

            if(!tools[button] || tools[button] instanceof tool.constructor) return;
            else if(tools[opposite] instanceof tool.constructor) {
                tools[opposite] = tools[button];
                if(typeof tools[opposite].id == 'number') toolList[tools[opposite].id].node.clr = toolColors[opposite];
                tools[button] = new tool.constructor();
                tools[button].id = i;
                toolList[i].node.clr = toolColors[button];
            } else {
                if(typeof tools[button].id == 'number') delete toolList[tools[button].id].node.clr;
                tools[button] = new tool.constructor();
                tools[button].id = i;
                toolList[i].node.clr = toolColors[button];
            }
        });
        renderList.push(toolNode);
        toolNode.id = 'tool_' + tool.name;
        toolY += 21 * scaleF;

        _i += 1;
    }

    toolY += 21 * scaleF * 1;

    for(const control of controls) {
        const controlNode = new TextNode(control.symbol, scaleF * 1.8, new Vec2(left - 2 * scaleF, top + toolY), 'right');
        const name = new TextNode(control.name, scaleF, new Vec2(left + 4 * scaleF, top + toolY + 3 * scaleF * 1.8));
        name.visible = false;
        controlNode.on('mouseenter', () => {
            document.body.style.cursor = 'pointer';
            name.visible = true;
        });
        controlNode.on('mouseexit', () => {
            document.body.style.cursor = 'default'
            name.visible = false;
        });
        controlNode.on('mousedown', () => {
            control.callback(controlNode, name);
        })
        renderList.push(controlNode, name);
        controlNode.id = 'control_' + control.name;
        toolY += 21 * scaleF;
    }

    infoNode = new TextNode('-1x, -1y, 0.2°C, VOID', scaleF, new Vec2(right - 8, top - 13 * scaleF), 'right');
    renderList.push(infoNode);

    const plopNode = new TextNode('plop 1.0', scaleF, new Vec2(left + 8, bottom + 4 * scaleF));
    plopNode.on('mouseenter', () => document.body.style.cursor = 'pointer');
    plopNode.on('mouseexit', () => document.body.style.cursor = 'default');
    plopNode.on('mousedown', () => window.open('https://github.com/Caltrop256/plop/'));
    renderList.push(plopNode);
    const caltropDevNode = new TextNode('caltrop.dev', scaleF, new Vec2(right - 8, bottom + 4 * scaleF), 'right');
    caltropDevNode.on('mouseenter', () => document.body.style.cursor = 'pointer');
    caltropDevNode.on('mouseexit', () => document.body.style.cursor = 'default');
    caltropDevNode.on('mousedown', () => window.open('https://caltrop.dev/'));
    renderList.push(caltropDevNode);

    const border = new Drawable([
        left, top,
        right, top,
        right, bottom,
        left, bottom
    ], [
        [0, 1],
        [1, 2],
        [2, 3],
        [3, 0]
    ], 1);
    const genEl = new Element(new Vec2());
    genEl.draw = ctx => border.drawAt(ctx, new Vec2());
    renderList.push(genEl);

    renderList[Element.getElementIndexById('cat_' + categorySelected)].clr = toolColors[0];
}

function createView(type, variable, length, isPointer = false) {
    const ptr = wasm.exports[variable].value
    const buf = new window[type + 'Array'](
        wasm.exports.memory.buffer,
        isPointer ? new Uint32Array(wasm.exports.memory.buffer, ptr, 1)[0] : ptr,
        length || 1
    );
    return buf;
}

void async function main() {
    fetch('sim.wasm')
    .then(r => r.arrayBuffer())
    .then(bytes => WebAssembly.instantiate(bytes, {
        env: {
            log: console.log,
            cos: Math.cos,
            sin: Math.sin,
            atan2: Math.atan2,
        }
    }))
    .then(({instance}) => {
        wasm = instance;
        const seed = new Uint32Array(6);
        crypto.getRandomValues(seed);
        readEnumArray();
        elementInBrush = lookup['SAND'].id;
        generateFavicon(catSymbols[2]);
        constructUI(renderList);
        wasm.exports.seed(...seed);
        setSize(4);
        loop();
    })
}();

function loop() {
    requestAnimationFrame(loop);
    eventhandler.tick();

    if(__memoryLen && wasm.exports.memory.buffer.byteLength != __memoryLen) {
        const view = createView('Uint8Clamped', 'imageData', canvas.width * canvas.height * 4, true);
        imageData = new ImageData(view, canvas.width, canvas.height);
    }
    __memoryLen = wasm.exports.memory.buffer.byteLength;

    wasm.exports.draw();
    if(!paused) wasm.exports.tick();
    ctx.putImageData(imageData, 0, 0);

    const explosionPower = wasm.exports.getFrameExplosionPower();
    if(explosionPower > frameExplosionPower) frameExplosionPower = explosionPower;
}

function generateFavicon(glyph) {
    const ctx = document.createElement('canvas').getContext('2d');
    ctx.canvas.width = 32;
    ctx.canvas.height = 32;
    ctx.strokeStyle = '#ffffff';
    Glyph.create(glyph, 3, new Vec2()).drawAt(ctx, new Vec2(1, 1));
    document.getElementById('favicon').href = ctx.canvas.toDataURL('image/png', 10);
}