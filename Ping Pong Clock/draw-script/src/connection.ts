import { Color } from 'pts';


export class Connection {
    
    private wsEndpoint = "ws://" + window.location.hostname + ":3000/websocket";
    private wsConnected = false;
    private socket: WebSocket;

    private connect() {
        console.log("Connecting to " + this.wsEndpoint);
        this.socket = new WebSocket(this.wsEndpoint);
        this.socket.onopen = (event) => {
            this.wsConnected = true;
            console.log("[open] Connection established");
            document.getElementById('ws-indicator').className = 'connected';
            document.getElementById('ws-indicator').innerHTML = 'Connected';
        };
        this.socket.onclose = (event) => {
            this.wsConnected = false;
            document.getElementById('ws-indicator').className = 'disconnected';
            document.getElementById('ws-indicator').innerHTML = 'Disconnected';
            if (event.wasClean) {
                console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
            } else {
                // e.g. server process killed or network down
                // event.code is usually 1006 in this case
                console.log('[close] Connection died - reconnect by reloading the page');
                setTimeout(this.connect.bind(this), 1000);
            }
        };
    
        this.socket.onerror = (event: Event):void => {
            this.wsConnected = false;
            console.error(`[error] ${event}`);
        };
    }
    constructor() {
        this.connect();
    }

    public setBallColor(idx: number, color: Color): boolean {
        let msg = "#" + this.componentToHex(idx) + this.componentToHex(color.r) + this.componentToHex(color.g) + this.componentToHex(color.b);
        return this.sendMessage(msg);
    }

    public switchMode(mode: number) {
        let msg = "s" + mode;
        this.sendMessage(msg);
    }
    
    private componentToHex(c) {
        var hex = c.toString(16);
        return hex.length == 1 ? "0" + hex : hex;
    }

    private sendMessage(msg: string): boolean {
        if (this.wsConnected) {
            this.socket.send(msg);
            return true;
        }
        else {
            console.debug("WS: Server disconnected, cannot send: " + msg);
            return false;
        }
    }
}


let wsEndpoint = "ws://" + window.location.hostname + ":8081";



function connect() {
    

    
}

connect();