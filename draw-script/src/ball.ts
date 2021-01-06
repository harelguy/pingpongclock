import { Color } from "pts";
import { Connection } from "./connection";

export class Ball {
    private i: number;
    private x: number;
    private y: number;
    private radius: number;
    private active: boolean = false;
    private currentHover = false;
    private color: Color;
    private noColor: Color = Color.fromHex('#000000');

    constructor(i: number, x: number, y: number, radius: number) {
        this.i = i;
        this.x = x;
        this.y = y;
        this.radius = radius;
    }

    public getX() {
        return this.x;
    }
    public getY() {
        return this.y;
    }
    public getRadius() {
        return this.radius;
    }
    public getIndex() {
        return this.i;
    }
    public hover() {
        if (this.currentHover)
            return;
        this.currentHover = true;
        this.active = ! this.active;
    }
    public setColor(color: Color, connection: Connection) {
        if (this.currentHover)
            return;
        this.currentHover = true;
        // this.active = ! this.active;
        if (connection.setBallColor(this.i, color)) {
            this.color = color;
        }
    }
    public getColor(): Color {
        return this.color;
    }
    public removeColor(color: Color, connection: Connection) {
        if (this.currentHover)
            return;
        this.currentHover = true;
        if(connection.setBallColor(this.i, this.noColor))
            this.color = color;
    }
    public notHover() {
        this.currentHover = false;
    }
    public isActive() {
        return this.active;
    }
}
