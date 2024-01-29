const socket = io();
let scale = 1;
let exTime = +new Date();
let gyroData;
let gamepadData;
let gamepadState = {};
let myPeerConnection;
let screenPosition;

const scaleSelect = document.getElementById("scale");
const fullBtn = document.getElementById("Fullscreen");
const screen = document.getElementById("screen");
const video = document.getElementById("video");
const gamepadSelect = document.getElementById("gamepad");
const screenPositionSelect = document.getElementById("position");
const roomName = "upad";

scaleSelect.addEventListener("change", () => (scale = scaleSelect.value));

// Gyro part
window.ondevicemotion = function (motion) {
  const gyroV = { x: scale * motion.rotationRate.alpha, z: scale * motion.rotationRate.beta, y: -scale * motion.rotationRate.gamma };
  const gyroH = { z: scale * motion.rotationRate.alpha, x: -scale * motion.rotationRate.beta, y: -scale * motion.rotationRate.gamma };

  ["x", "y", "z"].forEach((axis) => (document.getElementById(axis).textContent = `${axis}:${gyroV[axis]}`));

  gyroData = { ts: new Date().getTime(), gyro: screenPositionSelect.value == "v" ? gyroV : gyroH, acceleration: { x: 0, y: 0, z: 0 } };
};

// Gamepad part
window.addEventListener("gamepadconnected", handleGamepadConnected);
window.addEventListener("gamepaddisconnected", handleGamepadDisconnected);

function handleGamepadConnected(event) {
  const gamepad = event.gamepad;
  gamepadSelect.add(new Option(gamepad.id, gamepad.id));
  gamepadState[gamepad.index] = {};
  if (Object.keys(gamepadState).length === 1) updateGamepadState();
}

function handleGamepadDisconnected(event) {
  const gamepad = event.gamepad;
  delete gamepadState[gamepad.index];
}

const pollingInterval = 0;

function updateGamepadState() {
  const gamepads = navigator.getGamepads();
  const selectPad = gamepads.find((pad) => pad.id == gamepadSelect.value);
  const outputBtnValue = [];
  const outputAxeValue = [];
  const exGamepadState = JSON.parse(JSON.stringify(gamepadState));

  gamepads.forEach((gamepad) => {
    if (gamepad && gamepad.index == selectPad.index) {
      gamepad.buttons.forEach((button, index) => {
        gamepadState[gamepad.index][`button${index + 1}`] = button.value;
        outputBtnValue.push(button.value);
      });

      gamepad.axes.forEach((axis, index) => {
        gamepadState[gamepad.index][`axis${index + 1}`] = axis;
        outputAxeValue.push(axis);
      });
    }
  });


  const now = +new Date();

  if (JSON.stringify(gamepadState) !== JSON.stringify(exGamepadState)) {
    console.log(outputBtnValue, outputAxeValue, now - exTime);
    gamepadData = { ts: now, buttons: outputBtnValue, axes: outputAxeValue };
    socket.emit("data", { btn: outputBtnValue, axe: outputAxeValue });
  }

  exTime = now;
  if (Object.keys(gamepadState).length !== 0) setTimeout(updateGamepadState, pollingInterval);

}
fullBtn.addEventListener("click", (event) => {
  event.preventDefault();
  console.log("fullscreen");
  const requestFullScreen = screen.requestFullscreen || screen.mozRequestFullScreen || screen.webkitRequestFullscreen || screen.msRequestFullscreen;
  requestFullScreen.call(screen);
  const height = screen.offsetHeight;
  const width = (height / 9) * 16;
  video.style.width = `${width}px`;
});

// Soket join part
async function initCall() {
  await makeConnection();
}

async function handleWelcomeSubmit() {
  await initCall();
  socket.emit("join_room", roomName);
}

handleWelcomeSubmit();

// Socket part
socket.on("welcome", handleWelcome);

async function handleWelcome() {
  myDataChannel = myPeerConnection.createDataChannel("chat");
  myDataChannel.addEventListener("message", (event) => console.log(event.data));
  console.log("made data channel");
  const offer = await myPeerConnection.createOffer({ offerToReceiveAudio: true, offerToReceiveVideo: true });
  myPeerConnection.setLocalDescription(offer);
  console.log("sent the offer");
  socket.emit("offer", offer, roomName);
}

socket.on("offer", handleOffer);

async function handleOffer(offer) {
  myPeerConnection.addEventListener("datachannel", handleDataChannel);
  console.log("received the offer");
  myPeerConnection.setRemoteDescription(offer);
  const answer = await myPeerConnection.createAnswer();
  myPeerConnection.setLocalDescription(answer);
  socket.emit("answer", answer, roomName);
  console.log("sent the answer");
}

socket.on("answer", handleAnswer);

function handleAnswer(answer) {
  console.log("received the answer");
  myPeerConnection.setRemoteDescription(answer);
}

socket.on("ice", (ice) => {
  console.log("received candidate");
  myPeerConnection.addIceCandidate(ice);
});

function handleIce(data) {
  console.log("received candidate");
  socket.emit("ice", data.candidate, roomName);
}

// RTC part


function makeConnection() {
  myPeerConnection = new RTCPeerConnection({
    iceServers: [
      { urls: ["stun:stun.l.google.com:19302", "stun:stun1.l.google.com:19302", "stun:stun2.l.google.com:19302", "stun:stun3.l.google.com:19302", "stun:stun4.l.google.com:19302"] },
    ],
  });
  myPeerConnection.addEventListener("icecandidate", handleIce);
  myPeerConnection.addEventListener("addstream", handleAddStream);
}

function handleDataChannel(event) {
  myDataChannel = event.channel;
  myDataChannel.addEventListener("message", (event) => console.log(event.data));
}

function handleAddStream(data) {
  console.log("received the stream");
  video.srcObject = data.stream;
}

function datatransfer(data) {
  socket.emit("data", data);
}
