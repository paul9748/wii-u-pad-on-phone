const socket = io();
let scale = 1;
let exTime = +new Date();
let exGamepadState;
let gyroData;

const scaleSelect = document.getElementById("scale");
const fullBtn = document.getElementById("Fullscreen");
const screen = document.getElementById("screen");
const video = document.getElementById("video");
const roomName = "upad";

scaleSelect.addEventListener("change", handleScaleChange);

function handleScaleChange() {
  scale = scaleSelect.value;
  console.log(scale);
}
window.ondevicemotion = function (motion) {
  //x(red) z(blue) y(green)
  let gyroV = {
    x: scale * motion.rotationRate.alpha,
    z: scale * motion.rotationRate.beta,
    y: -scale * motion.rotationRate.gamma,
  };
  let gyroH = {
    z: scale * motion.rotationRate.alpha,
    x: -scale * motion.rotationRate.beta,
    y: -scale * motion.rotationRate.gamma,
  };

  document.getElementById("x").textContent = "x:" + gyroV.x;
  document.getElementById("y").textContent = "y:" + gyroV.y;
  document.getElementById("z").textContent = "z:" + gyroV.z;
  gyroData = {
    ts: new Date().getTime(),
    gyro: screen == "v" ? gyroV : gyroH,
    acceleration: {
      x: 0,
      y: 0,
      z: 0,
    },
  };
};

// Gamepad part
// 게임패드 입력 상태를 저장할 객체
let gamepadState = {};

// 게임패드 연결 이벤트 리스너
window.addEventListener("gamepadconnected", (event) => {
  const gamepad = event.gamepad;
  console.log(`게임패드가 연결되었습니다: ${gamepad.id}`);

  // 새로 연결된 게임패드의 상태를 초기화
  gamepadState[gamepad.index] = {};
});

// 게임패드 연결 해제 이벤트 리스너
window.addEventListener("gamepaddisconnected", (event) => {
  const gamepad = event.gamepad;
  console.log(`게임패드가 연결이 해제되었습니다: ${gamepad.id}`);
  // 연결 해제된 게임패드의 상태 삭제
  delete gamepadState[gamepad.index];
});

// 주기적으로 게임패드 상태 감지 및 출력
const pollingInterval = 0; // 밀리초 단위로 감지 주기 설정

function updateGamepadState() {
  const gamepads = navigator.getGamepads();
  exGamepadState = JSON.parse(JSON.stringify(gamepadState)); // 깊은 복사를 사용하여 exGamepadState를 생성
  for (const gamepad of gamepads) {
    if (gamepad) {
      // 게임패드의 모든 버튼 및 축을 순회하면서 상태 업데이트
      gamepadState[gamepad.index] = {};

      gamepad.buttons.forEach((button, index) => {
        gamepadState[gamepad.index][`button${index + 1}`] = button.value;
      });

      gamepad.axes.forEach((axis, index) => {
        gamepadState[gamepad.index][`axis${index + 1}`] = axis;
      });
    }
  }
  let now = +new Date();
  // 콘솔에 게임패드 상태 출력
  if (JSON.stringify(gamepadState) !== JSON.stringify(exGamepadState)) {
    // 객체 내부의 값을 비교
    console.log(gamepadState);
    let now = +new Date();
    console.log(now - exTime);
  }
  exTime = now;
  // 다음 프레임 업데이트를 요청
  setTimeout(updateGamepadState, pollingInterval);
}

fullBtn.addEventListener("click", function (event) {
  event.preventDefault();
  console.log("fullscreen");
  if (screen.requestFullscreen) {
    screen.requestFullscreen();
  } else if (screen.mozRequestFullScreen) {
    screen.mozRequestFullScreen();
  } else if (screen.webkitRequestFullscreen) {
    screen.webkitRequestFullscreen();
  } else if (screen.msRequestFullscreen) {
    screen.msRequestFullscreen();
  }
  let height = screen.offsetHeight;
  const width = (height / 9) * 16;
  video.style.width = width + "px";
});

// 최초 업데이트 시작
// updateGamepadState();

//Soket join part

async function initCall() {
  await makeConnection();
}

async function handleWelcomeSubmit(event) {
  await initCall();
  socket.emit("join_room", roomName);
}

handleWelcomeSubmit();

//Socket part

socket.on("welcome", async () => {
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
});

socket.on("offer", async (offer) => {
  myPeerConnection.addEventListener("datachannel", (event) => {
    myDataChannel = event.channel;
    myDataChannel.addEventListener("message", (event) =>
      console.log(event.data)
    );
  });
  console.log("received the offer");
  myPeerConnection.setRemoteDescription(offer);
  const answer = await myPeerConnection.createAnswer();
  myPeerConnection.setLocalDescription(answer);
  socket.emit("answer", answer, roomName);
  console.log("sent the answer");
});

socket.on("answer", (answer) => {
  console.log("received the answer");
  myPeerConnection.setRemoteDescription(answer);
});

socket.on("ice", (ice) => {
  console.log("received candidate");
  myPeerConnection.addIceCandidate(ice);
});
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

function handleIce(data) {
  console.log("sent candidate");
  socket.emit("ice", data.candidate, roomName);
}

function handleAddStream(data) {
  console.log("received the stream");
  video.srcObject = data.stream;
}

function datatransfer(data) {
  socket.emit("data", data);
}
