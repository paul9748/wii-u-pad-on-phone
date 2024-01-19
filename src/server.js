import http from "http";
import express from "express";

import { Server, Socket } from "socket.io";
import { instrument } from "@socket.io/admin-ui";

const app = express();

app.set("view engine", "pug");
app.set("views", __dirname + "\\views");
app.use("/public", express.static(__dirname + "/public"));
app.get("/", (req, res) => res.render("home"));
const handleListen = () => console.log("ws://localhost:3000");

const httpServer = http.createServer(app);
const wsServer = new Server(httpServer, {
  cor: {
    origin: ["https://admin.socket.io"],
    credentials: true,
  },
});
instrument(wsServer, { auth: false });
function publicRooms() {
  const {
    sockets: {
      adapter: { sids, rooms },
    },
  } = wsServer;
  const publicRooms = [];
  rooms.forEach((_, key) => {
    if (sids.get(key) === undefined) {
      publicRooms.push(key);
    }
  });
  return publicRooms;
}

function countRoom(roomName) {
  return wsServer.sockets.adapter.rooms.get(roomName)?.size;
}

wsServer.on("connection", (socket) => {
  socket["name"] = "anonymous";
  socket.onAny((event) => {
    console.log(`Socket event:${event}`);
  });
  socket.on("enter_room", (roomName, name, done) => {
    if (name != "") {
      socket["name"] = name;
    }
    socket.join(roomName);
    done();
    socket.to(roomName).emit("welcome", socket.name, countRoom(roomName));

    wsServer.sockets.emit("room_change", publicRooms());
  });
  socket.on("new_message", (msg, roomName, done) => {
    socket.to(roomName).emit("new_message", `${socket.name}:${msg}`);
    done();
  });
  socket.on("disconnecting", () => {
    socket.rooms.forEach((room) =>
      socket.to(room).emit("bye", socket.nickname, countRoom(room) - 1)
    );
  });
  socket.on("disconnect", () => {
    wsServer.sockets.emit("room_change", publicRooms());
  });
});

httpServer.listen(3000, handleListen);
