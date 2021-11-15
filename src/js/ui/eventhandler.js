class EventHandler {
    hoveringInPreviousTick = [];

    mx = -1;
    my = -1;

    isPhone = false;

    shiftKeyPressed = false;

    constructor() {
        window.addEventListener('contextmenu', e => e.preventDefault());
        window.addEventListener('mousemove', e => {
            if(this.isPhone) return;
            this.mx = e.clientX;
            this.my = e.clientY;
            this.tick();
        });

        window.addEventListener('mousedown', e => {
            if(this.isPhone) return;
            this.mx = e.clientX;
            this.my = e.clientY;

            let clickedOnElement = false;
            for(const element of renderList) {
                if(element.onmousedown && element.visible && element.pointInBoundary(this.mx, this.my)) {
                    element.onmousedown.apply(element, [e]);
                    clickedOnElement = true;
                }
            }

            if(e.button == 1) {
                const {x, y} = this.getCanvasCoords();
                const type = wasm.exports.getType(wasm.exports.getCell(x, y));
                if(type > lookup['EMPTY'].id && lookup[enumToString[type]].category != categories.hidden) {
                    elementInBrush = type;
                    const ind = Element.getElementIndexById('elementselect_' + lastSelected);
                    if(ind != null) delete renderList[ind].clr;
                    const catInd = Element.getElementIndexById('cat_' + categorySelected);
                    if(catInd != null) delete renderList[catInd].clr;
                    lastSelected = lastSelected = lookup[enumToString[elementInBrush]].name || enumToString[elementInBrush];;
                    categorySelected = Object.getOwnPropertyNames(categories)[lookup[enumToString[type]].category];
                    const sind = Element.getElementIndexById('elementselect_' + lastSelected);
                    if(sind != null) renderList[sind].clr = toolColors[0];
                    const scatInd = Element.getElementIndexById('cat_' + categorySelected);
                    if(scatInd != null) renderList[scatInd].clr = toolColors[0];
                }
            } else if(e.button <= 2) {
                const tool = tools[e.button == 0 ? 0 : 1];
                if(tool && !clickedOnElement && this.inCanvas()) {
                    tool.ondown(this.getCanvasCoords());
                }
            }
        });

        window.addEventListener('mouseup', e => {
            if(this.isPhone) return;
            this.mx = e.clientX;
            this.my = e.clientY;
            const tool = tools[e.button == 0 ? 0 : 1];
            if(tool) tool.onup(this.getCanvasCoords());
        })

        window.addEventListener('touchstart', e => {
            this.isPhone = true;
            this.mx = e.changedTouches[0].clientX;
            this.my = e.changedTouches[0].clientY;
            let clickedOnElement = false;
            for(const element of renderList) {
                if(element.onmousedown && element.visible && element.pointInBoundary(this.mx, this.my)) {
                    element.onmousedown.apply(element, [{button: 0, clientX: this.mx, clientY: this.my}]);
                    clickedOnElement = true;
                }
            }

            const tool = tools[0];
            if(tool && !clickedOnElement && this.inCanvas()) {
                tool.ondown(this.getCanvasCoords());
            }
        });

        window.addEventListener('touchmove', e => {
            this.isPhone = true;
            this.mx = e.changedTouches[0].clientX;
            this.my = e.changedTouches[0].clientY;
            this.tick();
        });

        window.addEventListener('touchend', e => {
            this.isPhone = true;
            this.mx = e.changedTouches[0].clientX;
            this.my = e.changedTouches[0].clientY;
            const tool = tools[0];
            if(tool) tool.onup(this.getCanvasCoords());
        })

        window.addEventListener('keypress', e => {
            if(/[1-5]/.test(e.key)) {
                wasm.exports.changeScene(+e.key);
            } else {
                const id = {
                    'd': 'tool_draw',
                    'e': 'tool_erase',
                    'l': 'tool_line',
                    'w': 'tool_wind',
                    ' ': 'control_pause',
                    '.': 'control_step',
                    'a': 'control_reset',
                    'c': 'control_resize',

                    's': 'control_save state',
                    'r': 'control_load state'
                }[e.key.toLowerCase()];
                if(id) {
                    const ind = Element.getElementIndexById(id);
                    if(ind) renderList[ind].onmousedown({button: 0});
                }
            }
        });

        window.addEventListener('keydown', e => e.key.toLowerCase() == 'shift' && (this.shiftKeyPressed = true));
        window.addEventListener('keyup', e => e.key.toLowerCase() == 'shift' && (this.shiftKeyPressed = false));

        window.addEventListener('wheel', e => {
            renderList[Element.getElementIndexById('tool_areaOfEffect')].onmousedown({button: e.deltaY < 0 ? 0 : 2});
        });
        window.addEventListener('dragover', e => e.preventDefault())
        window.addEventListener('drop', e => {
            e.preventDefault();
            const reader = new FileReader();
            reader.onloadend = () => {
                const res = new Uint8Array(reader.result);
                if(importData(res)) {
                    state = res;
                }
            }
            reader.readAsArrayBuffer(e.dataTransfer.files[0]);
        })
    }

    inCanvas() {
        const {top,left,right,bottom} = canvas.getBoundingClientRect();
        return this.mx > left && this.mx < right && this.my > top && this.my < bottom;
    }

    getCanvasCoords() {
        const {left, top, width, height} = canvas.getBoundingClientRect();
        return {
            x: ((this.mx - left) * (canvas.width / width)) | 0,
            y: ((this.my - top) * (canvas.height / height)) | 0
        }
    }

    tick() {
        const hovering = [];
        let isHoveringOverVisibleElement = false;
        for(const element of renderList) {
            if(element.pointInBoundary(this.mx, this.my)) {
                hovering.push(element);
                if(element.visible && !element.id.startsWith('bounding_')) isHoveringOverVisibleElement = true;
            }
        }

        outer2:
        for(let i = 0; i < this.hoveringInPreviousTick.length; ++i) {
            if(!this.hoveringInPreviousTick[i].onmouseexit || !this.hoveringInPreviousTick[i].visible) continue;
            for(let j = 0; j < hovering.length; ++j) {
                if(hovering[j] == this.hoveringInPreviousTick[i]) continue outer2;
            }
            this.hoveringInPreviousTick[i].onmouseexit.apply(hovering[i], hovering[i]);
        }

        outer1:
        for(let i = 0; i < hovering.length; ++i) {
            if(!hovering[i].onmouseenter || !hovering[i].visible) continue;
            for(let j = 0; j < this.hoveringInPreviousTick.length; ++j) {
                if(hovering[i] == this.hoveringInPreviousTick[j]) continue outer1;
            }
            hovering[i].onmouseenter.apply(hovering[i], hovering[i]);
        }

        if(!isHoveringOverVisibleElement && this.inCanvas()) {
            const coords = this.getCanvasCoords();
            for(const tool of tools) {
                tool.ontick(coords);
            }
        }

        this.hoveringInPreviousTick = hovering;
    }
}