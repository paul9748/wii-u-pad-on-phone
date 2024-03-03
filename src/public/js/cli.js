const socket = io();
let scale = 1;
let exTime = +new Date();
let gyroData = {};
let gamepadData = {};
gamepadData.buttons = Array(16).fill(0);
gamepadData.axes = Array(4).fill(0);
let gamepadState = {};
let myPeerConnection;
let screenPosition;
let startGyro = false;
let touchData;
const titleCut = document.getElementById("titleCut");
const bottomCut = document.getElementById("bottomCut");
const scaleSelect = document.getElementById("scale");
const fullBtn = document.getElementById("Fullscreen");
const screen = document.getElementById("screen");
const video = document.getElementById("video");
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const gamepadSelect = document.getElementById("gamepad");
const screenPositionSelect = document.getElementById("position");
const roomName = "upad";
scaleSelect.addEventListener("change", () => (scale = scaleSelect.value));
video.style.display = "none";
// Gyro part
window.ondevicemotion = function (motion) {
  startGyro = true;
  const gyroV = {
    x: scale * motion.rotationRate.alpha,
    z: scale * motion.rotationRate.beta,
    y: -scale * motion.rotationRate.gamma,
  };
  const gyroH = {
    z: scale * motion.rotationRate.alpha,
    x: -scale * motion.rotationRate.beta,
    y: -scale * motion.rotationRate.gamma,
  };

  gyroData = {
    ts: new Date().getTime(),
    gyro: screenPositionSelect.value == "v" ? gyroV : gyroH,
    acceleration: { x: 0, y: 0, z: 0 },
  };
  updateGamepadState();
  gamepadDataSand();
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
  const gamepads = navigator.getGamepads().filter((element) => {
    return element !== null;
  });
  const selectPad = gamepads.find((pad) => pad.id == gamepadSelect.value);
  const outputBtnValue = Array(16).fill(0);
  const outputAxeValue = Array(4).fill(0);
  const exGamepadState = JSON.parse(JSON.stringify(gamepadState));

  gamepads.forEach((gamepad) => {
    if (gamepad && gamepad.index == selectPad.index) {
      gamepad.buttons.forEach((button, index) => {
        gamepadState[gamepad.index][`button${index + 1}`] = button.value;
        outputBtnValue[index] = button.value;
      });

      gamepad.axes.forEach((axes, index) => {
        gamepadState[gamepad.index][`axes${index + 1}`] = axes;
        outputAxeValue[index] = axes;
      });
    }
  });

  const now = +new Date();

  if (JSON.stringify(gamepadState) !== JSON.stringify(exGamepadState)) {
    // console.log(outputBtnValue, outputAxeValue, now - exTime);
    gamepadData = { buttons: outputBtnValue, axes: outputAxeValue };
    gamepadDataSand();
  }

  exTime = now;
  //&& !startGyro
  if (Object.keys(gamepadState).length !== 0 && !startGyro)
    setTimeout(updateGamepadState, pollingInterval);
}

function gamepadDataSand() {
  let data;
  if (gamepadData == {}) {
    data = gyroData;
  } else {
    data = Object.assign(gyroData, {
      btn: gamepadData.buttons,
      axes: gamepadData.axes,
    });
  }
  socket.emit("data", data);
}
//pad data => a b x y l r l2 r2 sl st l3 r3
//pad axal => lh lv rh rv
fullBtn.addEventListener("click", async (event) => {
  event.preventDefault();
  console.log("fullscreen");

  const requestFullScreen =
    screen.requestFullscreen ||
    screen.mozRequestFullScreen ||
    screen.webkitRequestFullscreen ||
    screen.msRequestFullscreen;

  await requestFullScreen.call(screen);
});

function adjustCanvasSize() {
  const screenRect = screen.getBoundingClientRect();
  const aspectRatio = canvas.width / canvas.height;

  // Calculate the maximum width and height that maintains the aspect ratio
  let maxWidth = screenRect.width;
  let maxHeight = screenRect.height;

  if (maxWidth / maxHeight > aspectRatio) {
    maxWidth = maxHeight * aspectRatio;
  } else {
    maxHeight = maxWidth / aspectRatio;
  }

  canvas.style.width = `${maxWidth}px`;
  canvas.style.height = `${maxHeight}px`;
}

document.addEventListener("fullscreenchange", () => {
  if (document.fullscreenElement === screen) {
    adjustCanvasSize();
  }
});

// Adjust canvas size initially
adjustCanvasSize();

//touchpart

canvas.addEventListener("touchstart", (event) => {
  event.preventDefault();
  let titleCutValue = parseFloat(titleCut.value) || 0;
  let canvasLocation = canvas.getBoundingClientRect();
  console.log(
    `touchstart:${event.touches[0].clientX - canvasLocation.x}, ${
      event.touches[0].clientY - canvasLocation.y + titleCutValue
    }`
  );
  socket.emit(
    "touch_event",
    "mousedown",
    Math.round(event.touches[0].clientX - canvasLocation.x),
    Math.round(event.touches[0].clientY - canvasLocation.y + titleCutValue)
  );
});

canvas.addEventListener("touchmove", (event) => {
  event.preventDefault();
  let titleCutValue = parseFloat(titleCut.value) || 0;
  let canvasLocation = canvas.getBoundingClientRect();
  socket.emit(
    "touch_event",
    "mousemove",
    Math.round(event.touches[0].clientX - canvasLocation.x),
    Math.round(event.touches[0].clientY - canvasLocation.y + titleCutValue)
  );
  console.log(
    `touchmove:${event.touches[0].clientX - canvasLocation.x}, ${
      event.touches[0].clientY - canvasLocation.y + titleCutValue
    }`
  );
});

canvas.addEventListener("touchend", (event) => {
  event.preventDefault();
  let titleCutValue = parseFloat(titleCut.value) || 0;
  let canvasLocation = canvas.getBoundingClientRect();
  console.log(
    Math.round(event.changedTouches[0].clientX - canvasLocation.x),
    Math.round(
      event.changedTouches[0].clientY - canvasLocation.y + titleCutValue
    )
  );
  socket.emit(
    "touch_event",
    "mouseup",
    Math.round(event.changedTouches[0].clientX - canvasLocation.x),
    Math.round(
      event.changedTouches[0].clientY - canvasLocation.y + titleCutValue
    )
  );
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
  const offer = await myPeerConnection.createOffer({
    offerToReceiveAudio: true,
    offerToReceiveVideo: true,
  });
  myPeerConnection.setLocalDescription(offer);
  console.log("sent the offer");
  socket.emit("offer", offer, roomName);
}

socket.on("offer", handleOffer);

async function handleOffer(offer) {
  // myPeerConnection.addEventListener("datachannel", handleDataChannel);
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
      {
        urls: [
          "stun:stun.l.google.com:19302",
          "stun:stun1.l.google.com:19302",
          "stun:stun2.l.google.com:19302",
          "stun:stun3.l.google.com:19302",
          "stun:stun4.l.google.com:19302",
        ],
      },
    ],
  });
  myPeerConnection.addEventListener("icecandidate", handleIce);
  myPeerConnection.addEventListener("addstream", handleAddStream);
}

// function handleDataChannel(event) {
//   myDataChannel = event.channel;
//   myDataChannel.addEventListener("message", (event) => console.log(event.data));
// }

function handleAddStream(data) {
  console.log("received the stream");
  process.options.length = 0;
  video.srcObject = data.stream;
  drawFrame();
}
async function drawFrame() {
  let settings = video.srcObject.getVideoTracks()[0].getSettings();
  canvas.width = settings.width;
  canvas.height = settings.height - titleCut.value - bottomCut.value;
  // canvas.style.width = `${settings.width}px`;
  // canvas.style.height = `${settings.height - titleCut.value - bottomCut.value}px`;
  ctx.drawImage(video, 0, -titleCut.value, settings.width, settings.height);
  window.requestAnimationFrame(drawFrame);
}

function datatransfer(data) {
  socket.emit("data", data);
}
