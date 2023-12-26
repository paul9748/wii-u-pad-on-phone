import http from "http";
import express from "express";
import { WebSocketServer } from "ws";


const app = express();

app.set("view engine", "pug");
app.set("views", __dirname + "\\views");
app.use("/public", express.static(__dirname +"/public"));
app.get("/", (req, res) => res.render("home"));

const handleListen = () => console.log("ws://localhost:3000");

const server = http.createServer(app) ;

const wss = new WebSocketServer({ server });

const sockets = []

wss.on("connection",(socket) =>{
    sockets.push(socket);
    console.log("connecting to browser");
    socket.on("close", () =>{console.log("disconnected from browser");});
    socket.on("message", (message)=>{
        sockets.forEach(aSocket =>{
        aSocket.send(message.toString());
    })
    });

});

server.listen(3000, handleListen);
// app.listen(3000,handleListen) ;