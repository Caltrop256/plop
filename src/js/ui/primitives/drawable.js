class Drawable {
    constructor(points, connections, scale = 1) {
        this.points = [];
        for(let i = 0; i < points.length; i += 2) {
            this.points.push(new Point(points[i] * scale, points[i + 1] * scale));
        }

        this.parts = [];
        this.indC = connections;
        this.buildParts(scale);
    }

    buildParts(scale) {
        this.parts.length = 0;
        for(const connection of this.indC) {
            if(connection.length == 3) this.parts.push(new Primitive(this.points[connection[0]], this.points[connection[1]], this.points[connection[2]], scale));
            else {
                this.parts.push(new Primitive(this.points[connection[0]], this.points[connection[1]], null, scale));
                if(Math.random() > 0.5) this.parts.push(new Primitive(this.points[connection[1]], this.points[connection[0]], null, scale));
            }
        }
    }

    drawAt(ctx, pos) {
        ctx.beginPath();
        for(const primitive of this.parts) {
            primitive.skew(pos);
            primitive.draw(ctx);
        }
        for(const primitive of this.parts) primitive.reset();
        ctx.stroke();
        ctx.closePath();
    }
}