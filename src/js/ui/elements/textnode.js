class TextNode extends Element {
    static shake = 0;
    constructor(text, scale, position, justify = 'left') {
        super(position);
        this.scale = scale;
        this.justify = justify;
        this._text = '';
        this.changeText(text);
        this._text = text;
    }

    changeText(text) {
        if(text == this._text) return;
        this._text = text;
        const lines = text.toUpperCase().split('\n');
        this.glyphs = [];
        let largestOffsetX = 0;
        for(let y = 0; y < lines.length; ++y) {
            let ox = 0;
            for(let x = 0; x < lines[y].length; ++x) {
                const char = lines[y].charAt(x);
                this.glyphs.push({
                    drawable: Glyph.create(char, this.scale),
                    pivot: new Vec2(ox, 0)
                });
                ox += Glyph.kerning[char] * this.scale + 2;
            }
            if(ox > largestOffsetX) largestOffsetX = ox;
        }
        this.left = this.pivot.x;
        this.right = this.pivot.x + largestOffsetX;
        this.top = this.pivot.y;
        this.bottom = this.pivot.y + 10 * lines.length * this.scale;

        const width = this.width;
        switch(this.justify) {
            case 'right' :
                this.left -= width;
                this.right -= width;
                break;
            case 'center' :
                this.left -= width / 2;
                this.right -= width / 2;
        }
    }

    draw(ctx) {
        if(this.clr) ctx.strokeStyle = this.clr;
        for(const glyph of this.glyphs) {
            glyph.drawable.drawAt(ctx, new Vec2(this.left, this.top).add(glyph.pivot).add(Vec2.random().mult(TextNode.shake)))
        };
        if(this.clr) ctx.strokeStyle = '#ffffff';
    }
}