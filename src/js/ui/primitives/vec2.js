class Vec2 {
    static from(vec2) {
        return new Vec2(vec2.x, vec2.y);
    }

    static random() {
        return new Vec2(Math.random() * 2 - 1, Math.random() * 2 - 1)
    }

    constructor(x,y) {
        this.x = x || 0;
        this.y = y || 0;
    }

    add(vec2) {
        this.x += vec2.x;
        this.y += vec2.y;
        return this;
    }

    mult(n) {
        this.x *= n;
        this.y *= n;
        return this;
    }

    toString() {
        return `${this.x}, ${this.y}`
    }
}