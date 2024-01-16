import http from "http";
import express from "express";
import { Server } from "socket.io";


const app = express();

app.set("view engine", "pug");
app.set("views", __dirname + "\\views");
app.use("/public", express.static(__dirname + "/public"));
app.get("/", (req, res) => res.render("home"));

const handleListen = () => console.log("ws://localhost:3000");

const httpServer = http.createServer(app);

const wsServer = new Server(httpServer);

wsServer.on("connection", (socket) => {
    console.log(socket)
})


// const sockets = []

// wss.on("connection", (socket) => {
//     sockets.push(socket);
//     socket["nickname"] = "Anonymous";
//     console.log("connecting to browser");
//     socket.on("close", () => { console.log("disconnected from browser"); });
//     socket.on("message", (msg) => {
//         const message = JSON.parse(msg.toString());
//         console.log(message);
//         switch (message.type) {
//             case "new_message":
//                 sockets.forEach(aSocket => {
//                     aSocket.send(`${socket.nickname}: ${message.payload}`);
//                 });
//             case "nickname":
//                 console.log(message.payload)
//                 socket["nickname"] = message.payload;
//         };

//     });

// });

httpServer.listen(3000, handleListen);
// app.listen(3000,handleListen) ;