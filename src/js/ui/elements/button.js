class Button extends Element {
    constructor(label, scale, position) {
        super(position);
        const padding = 4 * scale;
        this.label = new TextNode(label, scale, new Vec2(position.x + padding, position.y + padding));

        this.border = new Drawable([
            0, 0,
            this.label.width + padding * 2, 0,
            this.label.width + padding * 2, this.label.height + padding * 2,
            0, this.label.height + padding * 2
        ], [
            [0, 1],
            [1, 2],
            [2, 3],
            [3, 0]
        ]);

        this.left = this.label.left - padding;
        this.right = this.label.right + padding;
        this.top = this.label.top - padding;
        this.bottom = this.label.bottom + padding;
    }

    draw(ctx) {
        if(this.clr) ctx.strokeStyle = this.clr;
        this.label.draw(ctx);
        this.border.drawAt(ctx, this.pivot);
        if(this.clr) ctx.strokeStyle = '#ffffff';
    }
}