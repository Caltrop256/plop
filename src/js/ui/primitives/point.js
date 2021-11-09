class Point extends Vec2 {
    static roughness = 0.5;
    bSkewed = false;
    constructor(x, y) {
        super(x, y);
        this.c = new Vec2(x, y);
    }

    reset() {
        this.bSkewed = false;
        this.x = this.c.x;
        this.y = this.c.y;
    }

    skew(n = 1) {
        this.bSkewed = true;
        this.x += (Math.random() * 2 - 1) * Point.roughness * n;
        this.y += (Math.random() * 2 - 1) * Point.roughness * n;
        return this;
    }
}