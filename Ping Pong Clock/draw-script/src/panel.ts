import {Ball} from "./ball"
import { Space } from "pts";

export class Panel {
    private colBalls: number[] = [1, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3];
    private ballsInCol: number;
    private canvasHeight: number;
    private balls: Ball[] = [];
    private numBalls = 128;
    private numRows = 7;
    private radius = 20;
    

    constructor (space: Space) {
        this.radius = window.innerWidth / 45;
        this.canvasHeight = this.numRows * this.radius * 2; //space.height;
        
        for (let i = 0; i < this.numBalls; i++) {
            let ballCol = 0;
            let acc = 0;
            let lastAcc = 0;
            while (i >= acc) {
                lastAcc = acc;
                acc += this.colBalls[ballCol];
                this.ballsInCol = this.colBalls[ballCol];
                ballCol++;
            }

            ballCol--;
            let ballRow = (i - lastAcc);
            let offsetX = Math.pow(-1, ballRow + 1) * this.radius / 2;
            if (ballCol < 2) {
                offsetX = offsetX * (-1);
            }
            offsetX -= this.radius / 2;
            let offsetY = (7 - this.colBalls[ballCol]) * this.radius;

            let y = (ballCol % 2 == 0)
                ? ballRow * this.radius * 2 + this.radius + offsetY
                : this.canvasHeight - (ballRow * this.radius * 2 + this.radius + offsetY);

            let x = ballCol * this.radius * 2 + this.radius + offsetX;

            // console.log("Rendering ball " + i + " at logical ( " + ballCol + " , " + ballRow + ", physical (" + x + " , " + y + " ) offsetX = " + offsetX);
            this.balls[i] = new Ball(i, x, y, this. radius);
        }
    }

    public getBalls(): Ball[] {
        return this.balls;
    }
}