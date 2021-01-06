import { Space, Group, Color, Pt, CanvasSpace, Circle, CanvasForm } from "pts";
import { runInThisContext } from "vm";

export enum PalletteMode {Draw, Erase};

interface colorCircle {
    hex: string,
    y: number,
    current: boolean
}

export class Pallette {
    
    private mode: PalletteMode = PalletteMode.Draw;
    private colorCircles: colorCircle[] = [];
    private currentColor: Color = Color.fromHex("#ffffff");
    private activeBorderColor = '#000000';
    private inactiveBorderColor = '#ffffff';
    private radius = 20;
    private activeColorScale = 1.2;
    private eraser: colorCircle;
    private backgroundColor: Color;

    constructor (space: CanvasSpace, backgroundColor: string, colorList: string[]) {
        this.radius = this.radius = window.innerWidth / 45;
        this.backgroundColor = Color.fromHex(backgroundColor);
        this.eraser = {
            hex: '#000000',
            y: 2 * this.radius,
            current:  false
        }
        colorList.map( (hex: string, idx: number) => {
            this.colorCircles.push(
                {
                    hex: hex,
                    y: ((idx + 1) * 3 + 2) * this.radius, // space.height - radius,
                    current: idx == 0
                }
            );
        })
    }

    render(space: CanvasSpace, form: CanvasForm) {
        form.fill(this.backgroundColor.hex)
            .stroke(this.mode == PalletteMode.Erase ? this.activeBorderColor : this.inactiveBorderColor)
            .circle(Circle.fromCenter([space.width - 2 * this.radius, this.eraser.y], this.radius));
        this.colorCircles.forEach( (colorCircle: colorCircle) => {
            let radius = this.mode == PalletteMode.Draw && colorCircle.current ? this.radius * this.activeColorScale : this.radius;
            let circle = Circle.fromCenter([space.width - 2 * this.radius, colorCircle.y], radius);
            form.fill(colorCircle.hex)
                .stroke(this.mode == PalletteMode.Draw && colorCircle.current ? this.activeBorderColor : this.inactiveBorderColor)
                .circle(circle);
        });
    }

    getCurrentColor(): Color {
        return this.currentColor;
    }

    getBackgroundColor(): Color {
        return this.backgroundColor;
    }

    selectColor(space: CanvasSpace) {

        if (Circle.withinBound(Circle.fromCenter([space.width - 2 * this.radius, this.eraser.y], this.radius), space.pointer)) {
            this.mode = PalletteMode.Erase;
            this.colorCircles.forEach( (colorCircle: colorCircle) => colorCircle.current = false);
        } else {
            this.colorCircles.forEach( (colorCircle: colorCircle) => {
                let circle = Circle.fromCenter([space.width - 2 * this.radius, colorCircle.y], this.radius)
                if (Circle.withinBound(circle, space.pointer)) {
                    this.mode = PalletteMode.Draw;
                    this.currentColor = Color.fromHex(colorCircle.hex);
                    this.colorCircles.forEach( (colorCircle: colorCircle) => colorCircle.current = false);
                    colorCircle.current = true;
                }
            });
        }
    }

    getMode(): PalletteMode {
        return this.mode;
    }
}