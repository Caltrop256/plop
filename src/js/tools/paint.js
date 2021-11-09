class PaintTool {

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
        return wasm.exports.applyPaint(mx, my, elementInBrush, areaOfEffect);
    }
}