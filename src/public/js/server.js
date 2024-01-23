const socket = io();

const selectBtn = document.getElementById("selectdisplay");

let myStream;
let myPeerConnection;



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
    try {
        myStream = await navigator.mediaDevices.getDisplayMedia(
            displayMediaOptions
        );
        myFace.srcObject = myStream;
    } catch (e) {
        console.log(e);
    }
}

async function handledisplayChange() {
    await getMedia();
    if (myPeerConnection) {
        const videoTrack = myStream.getVideoTracks()[0];
        const videoSender = myPeerConnection
            .getSenders()
            .find((sender) => sender.track.kind === "video");
        videoSender.replaceTrack(videoTrack);
    }
}



selectBtn.addEventListener("click", handledisplayChange);


//connect part


async function initCall() {
    welcome.hidden = true;
    call.hidden = false;
    await getMedia();
    makeConnection();
}


async function handleWelcomeSubmit(event) {
    event.preventDefault();
    const input = welcomeForm.querySelector("input");
    await initCall();
    socket.emit("join_room", input.value);
    roomName = input.value;
    input.value = "";
}


