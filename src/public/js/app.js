<<<<<<< HEAD
const socket = io();
const welcome = document.getElementById("welcome");
const form = welcome.querySelector("form");
const room = document.getElementById("room");

room.hidden = true;

let roomName;

function addMessage(message) {
  const ul = room.querySelector("ul");
  const li = document.createElement("li");
  li.innerText = message;
  ul.appendChild(li);
}

function showRoom() {
  welcome.hidden = true;
  room.hidden = false;
  const h3 = room.querySelector("h3");
  const msg = room.querySelector("#msg");
  h3.innerText = `room ${roomName}`;
  msg.addEventListener("submit", handleMessageSubmit);
}

function handleRoomSubmit(event) {
  event.preventDefault();
  const input = form.querySelector("#room_input");
  const input_name = form.querySelector("#name_input");
  socket.emit("enter_room", input.value, input_name.value, showRoom);
  roomName = input.value;
  input.value = "";
}

function handleMessageSubmit(event) {
  event.preventDefault();
  const input = room.querySelector("#msg input");
  const message = input.value;
  socket.emit("new_message", message, roomName, () => {
    addMessage(`you:${message}`);
  });
  input.value = "";
}

socket.on("welcome", (name, newCount) => {
  const h3 = room.querySelector("h3");
  h3.innerText = `Room ${roomName} (${newCount})`;
  addMessage(`${name} joined`);
});

socket.on("bye", (name, newCount) => {
  const h3 = room.querySelector("h3");
  h3.innerText = `Room ${roomName} (${newCount})`;
  addMessage(`${name} left`);
});

socket.on("room_change", (rooms) => {
  const roomList = welcome.querySelector("ul");
  roomList.innerHTML = "";
  if (rooms.length === 0) {
    return;
  }

  rooms.forEach((room) => {
    const li = document.createElement("li");
    li.innerText = room;
    roomList.append(li);
  });
});

socket.on("new_message", (msg) => addMessage(msg));
form.addEventListener("submit", handleRoomSubmit);
=======
const socket = io();
>>>>>>> efd94d09b74a6e2cc10c310baf11e02f97a90699
