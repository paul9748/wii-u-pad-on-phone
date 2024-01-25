const socket = io();
let scale = 1;
let exTime = + new Date();;

const scaleSelect = document.getElementById("scale");
const fullBtn = document.getElementById("full");
const myFace = document.getElementById("myFace");
const roomName = "upad";

scaleSelect.addEventListener("change", handleScaleChange);

function handleScaleChange() {
    scale = scaleSelect.value;
    console.log(scale);
}
window.ondevicemotion = function (motion) {
    //x(red) z(blue) y(green) 
    var gyroV = {
        x: scale * motion.rotationRate.alpha,
        z: scale * motion.rotationRate.beta,
        y: -scale * motion.rotationRate.gamma,
    }
    var gyroH = {
        z: scale * motion.rotationRate.alpha,
        x: -scale * motion.rotationRate.beta,
        y: -scale * motion.rotationRate.gamma,
    }

    document.getElementById("x").textContent = 'x:' + gyroV.x
    document.getElementById("y").textContent = 'y:' + gyroV.y
    document.getElementById("z").textContent = 'z:' + gyroV.z
    var data = {
        ts: new Date().getTime(),
        gyro: screen == 'v' ? gyroV : gyroH,
        acceleration: {
            x: 0,
            y: 0,
            z: 0
        }
    }
    // console.log(gyroV);
    set
};

// Gamepad part
// 게임패드 입력 상태를 저장할 객체
const gamepadState = {};

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
    const exGamepadState = gamepadState;
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

    // 콘솔에 게임패드 상태 출력
    if (gamepadState != exGamepadState) {
        console.log(gamepadState);
        let now = + new Date();
        console.log(now - exTime);
        exTime = now;
    }

    // 다음 프레임 업데이트를 요청
    setTimeout(updateGamepadState, pollingInterval);
}


fullBtn.addEventListener("click", function (event) {
    event.preventDefault();

    if (myFace.requestFullscreen) {
        myFace.requestFullscreen();
    } else if (myFace.mozRequestFullScreen) {
        myFace.mozRequestFullScreen();
    } else if (myFace.webkitRequestFullscreen) {
        myFace.webkitRequestFullscreen();
    } else if (myFace.msRequestFullscreen) {
        myFace.msRequestFullscreen();
    }
});

// 최초 업데이트 시작
updateGamepadState();


//Soket join part

async function initCall() {
    makeConnection();
}

async function handleWelcomeSubmit(event) {
    await initCall();
    socket.emit("join_room", roomName);

}


handleWelcomeSubmit();



//Socket part

socket.on("welcome", async () => {
    const offer = await myPeerConnection.createOffer();
    myPeerConnection.setLocalDescription(offer);
    console.log("sent the offer");
    socket.emit("offer", offer, roomName);
});

socket.on("offer", async (offer) => {
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
    console.log("make connection");
}

function handleIce(data) {
    console.log("sent candidate");
    socket.emit("ice", data.candidate, roomName);
}

function handleAddStream(data) {
    console.log("received the stream");
    myFace.srcObject = data.stream;
}
