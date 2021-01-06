import './style.css'
import { CanvasSpace, Create, Group, Polygon, Pt, GroupLike, Circle, Color } from 'pts';
import { Panel } from './panel';
import { Ball } from './ball';
import { Pallette, PalletteMode } from './pallette';
import { Connection } from './connection';

// Initiate Space and Form
const bgColor = '#09F';
const space = new CanvasSpace('#pts').setup({ bgcolor: bgColor, resize: true, retina: true });
const form = space.getForm();
const connection = new Connection();

let panel: Panel;
let pallette: Pallette;
let palletteRendered = false;
let colorList: string[] = ["#ffffff", "#ff0000", "#00ff00", "#0000ff"]
space.add({
  start: () => {
    panel = new Panel(space);
    pallette = new Pallette(space, bgColor, colorList);
  },

  animate: () => {
    pallette.render(space, form);
    pallette.selectColor(space);


    let currentColor: Color = pallette.getCurrentColor();
    panel.getBalls().map( (ball: Ball, idx: number) => {
      let pt = new Pt(ball.getX(), ball.getY(), ball.getRadius())
      let circle = Circle.fromCenter([ball.getX(), ball.getY()], ball.getRadius());
      if (Circle.withinBound(circle, space.pointer)) {
        if (pallette.getMode() == PalletteMode.Draw)
          ball.setColor(currentColor, connection);
        else
          ball.removeColor(pallette.getBackgroundColor(), connection);
      } else {
        ball.notHover();
      }
      form.fill(ball.getColor() == undefined ? bgColor : ball.getColor().hex).stroke("#ffffff").circle(circle);
    });
      
  },
});

// bind mouse events and play animation
space.bindMouse().bindTouch().play();

let buttons: NodeList = document.querySelectorAll("a.btn");
buttons.forEach( (button: Node) => {
  console.log('Adding event listener to: ', button);
  button.addEventListener('click', function(event) {
    console.log(event);
    let target = <HTMLAnchorElement>event.target;
    let href: string =  target.href;
    console.log('href: ' + href);
    var opts = {
      method: 'GET',      
      headers: {}
    };
    fetch(href, opts).then( (response) => {
      console.log(response);
      document.querySelectorAll("a.btn").forEach ( (button: Element) => button.classList.remove('selected') );
      target.classList.add('selected');
    });

    event.preventDefault ? event.preventDefault() : event.returnValue = false;
  })
});

// function doOnOrientationChange() {
//   switch(window.orientation) {  
//     case -90: case 90:
//       console.log('landscape');
//       document.querySelectorAll('.landscape-only').forEach((el: HTMLElement) => {el.style.display='block'});
//       document.querySelectorAll('.portrait-only').forEach((el: HTMLElement) => {el.style.display='none'});
//       break; 
//     default:
//       console.log('portrait');
//       document.querySelectorAll('.landscape-only').forEach((el: HTMLElement) => {el.style.display='none'});
//       document.querySelectorAll('.portrait-only').forEach((el: HTMLElement) => {el.style.display='block'});
//       break; 
//   }
// }

// window.addEventListener('orientationchange', doOnOrientationChange);
// doOnOrientationChange();