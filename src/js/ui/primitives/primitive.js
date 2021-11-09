const range = (t, min, max) => min + t * (max - min);

class Primitive {
    constructor(p0, p1, control, scale) {
        this.p0 = p0;
        this.p1 = p1;
        if(control) this.control = control;
        else this.control = new Point(
            range(0.8, this.p0.x, this.p1.x),
            range(0.8, this.p0.y, this.p1.y)
        );
        this.skewFactor = /*scale **/ 1;
    }

    draw(ctx) {
        ctx.moveTo(this.p0.x, this.p0.y);
        ctx.quadraticCurveTo(this.control.x, this.control.y, this.p1.x, this.p1.y)
    }

    skew(pos) {
        if(!this.p0.bSkewed) this.p0.skew(this.skewFactor).add(pos);
        if(!this.p1.bSkewed) this.p1.skew(this.skewFactor).add(pos);
        if(!this.control.bSkewed) this.control.skew(this.skewFactor * 3).add(pos);
    }

    reset() {
        this.p0.reset();
        this.p1.reset();
        this.control.reset();
    }
}