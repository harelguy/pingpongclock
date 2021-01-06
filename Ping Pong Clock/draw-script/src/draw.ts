import { CanvasSpace, Pt, Circle, Color, CanvasForm } from 'pts';
import { Panel } from './panel';
import { Ball } from './ball';
import { Pallette, PalletteMode } from './pallette';
import { Connection } from './connection';

export class Draw {
    // Initiate Space and Form
    private bgColor = '#09F';
    private space: CanvasSpace; // = new CanvasSpace('#pts').setup({ bgcolor: this.bgColor, resize: true, retina: true });
    private form: CanvasForm; // = this.space.getForm();
    private connection: Connection; 

    private panel: Panel;
    private pallette: Pallette;
    private palletteRendered = false;
    private colorList: string[] = ["#ffffff", "#000000", "#ff0000", "#00ff00", "#0000ff"]

    constructor() {
        this.space = new CanvasSpace('#pts').setup({ bgcolor: this.bgColor, resize: true, retina: true });
        console.log(this.space);
        this.form = this.space.getForm();
        this.connection = new Connection();

        this.space.add({
            start: () => {
                this.panel = new Panel(this.space);
                this.pallette = new Pallette(this.space, this.bgColor, this.colorList);
            },
        
            animate: () => {
                this.pallette.render(this.space, this.form);
                this.pallette.selectColor(this.space);
        
                console.log(this.space.pointer);
        
                let currentColor: Color = this.pallette.getCurrentColor();
                this.panel.getBalls().map( (ball: Ball, idx: number) => {
                    let pt = new Pt(ball.getX(), ball.getY(), ball.getRadius())
                    let circle = Circle.fromCenter([ball.getX(), ball.getY()], ball.getRadius());
                    if (Circle.withinBound(circle, this.space.pointer)) {
                        if (this.pallette.getMode() == PalletteMode.Draw)
                            ball.setColor(currentColor, this.connection);
                        else
                            ball.removeColor(this.pallette.getBackgroundColor(), this.connection);
                    } else {
                        ball.notHover();
                    }
                    this.form.fill(ball.getColor() == undefined ? this.bgColor : ball.getColor().hex).stroke("#ffffff").circle(circle);
                });
                
            },
            });
        
            // bind mouse events and play animation
            
    }
 
    public start() {
        this.space.bindMouse().bindTouch().play();
    }
}