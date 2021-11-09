class WindTool {

    pressed = false;
    mx = -1; my = -1;

    ondown({x,y}) {
        this.mx = x;
        this.my = y;
        this.pressed = true;
    }

    onup() {
        this.pressed = false;
    }

    ontick({x, y}) {
        if(this.pressed) {
            let pvx = this.mx;
            let pvy = this.my;
            line(this.mx, this.my, x, y, (cx, cy) => {
                wasm.exports.fluidVelocity(cx, cy, (cx - pvx) * areaOfEffect * 2.5, (cy - pvy) * areaOfEffect * 2.5 + 1);
                pvx = cx;
                pvy = cy;
            });
            this.mx = x;
            this.my = y;
        }
    }
}