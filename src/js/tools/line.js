class LineTool {
    drawing = false;
    anchorX = -1; anchorY = -1;
    tAX = -1; tAY = -1;
    mx = -1; my = -1;

    constructor() {
        const id = Element.getElementIndexById('tool_line_draw');
        if(id != null) {
            this.el = renderList[id];
        } else {
            this.el = new Element(new Vec2());
            this.el.id = 'tool_line_draw';
            renderList.push(this.el);
        }
        this.el.draw = ctx => {
            if(this.drawing) {
                new Drawable([
                    this.anchorX, this.anchorY,
                    this.mx, this.my
                ], [[0, 1]], 1).drawAt(ctx, new Vec2());
            }
        };
    }

    ondown({x,y}) {
        this.tAX = x;
        this.tAY = y;
        this.mx = eventhandler.mx;
        this.my = eventhandler.my;
        this.anchorX = this.mx;
        this.anchorY = this.my;

        this.drawing = true;
    }

    onup({x,y}) {
        this.drawing = false;
        if(this.tAX != -1 && this.tAY != -1) line(this.tAX, this.tAY, x, y, this.applyPaint);
        this.tAX = -1;
        this.tAY = -1;
    }

    ontick({x, y}) {
        this.mx = eventhandler.mx;
        this.my = eventhandler.my;
    }

    applyPaint(mx, my) {
        wasm.exports.applyPaint(mx, my, elementInBrush, areaOfEffect);
    }
}