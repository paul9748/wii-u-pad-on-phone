const socket = io();

const selectBtn = document.getElementById("selectdisplay");
const myFace = document.getElementById("vod");
const fullBtn = document.getElementById("full");

let myStream;
let myPeerConnection;

const roomName = "upad";



const displayMediaOptions = {
    video: {
        displaySurface: "browser",
    },
    audio: {
        suppressLocalAudioPlayback: false,
    },
    preferCurrentTab: false,
    selfBrowserSurface: "exclude",
    systemAudio: "include",
    surfaceSwitching: "include",
    monitorTypeSurfaces: "include",
};


async function getMedia() {
    console.log("get media");
    try {
        myStream = await navigator.mediaDevices.getDisplayMedia(
            displayMediaOptions
        );
        console.log("get media success");
        myFace.srcObject = myStream;
        // console.log(myStream);
    } catch (e) {
        console.log(e);
    }
}

async function handledisplayChange(event) {
    event.preventDefault();
    await getMedia();
    if (myPeerConnection) {
        const videoTrack = myStream.getVideoTracks()[0];
        const videoSender = myPeerConnection
            .getSenders()
            .find((sender) => sender.track.kind === "video");
        videoSender.replaceTrack(videoTrack);
        console.log("videoSender done");
    }
}



selectBtn.addEventListener("click", handledisplayChange);
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
//connect part


async function initCall() {
    // welcome.hidden = true;
    // call.hidden = false;
    console.log("init call");
    await getMedia();
    makeConnection();
}


async function handleWelcomeSubmit() {
    await initCall();
    socket.emit("join_room", roomName);
}

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
    myStream
        .getTracks()
        .forEach((track) => myPeerConnection.addTrack(track, myStream));
    console.log("make connection");
}


function handleIce(data) {
    console.log("sent candidate");
    socket.emit("ice", data.candidate, roomName);
}

handleWelcomeSubmit();


socket.on("welcome", async () => {
    try {
        const offer = await myPeerConnection.createOffer();
        myPeerConnection.setLocalDescription(offer);
        console.log("sent the offer");
        socket.emit("offer", offer, roomName);
    } catch (e) {
        console.log(e)
    }

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