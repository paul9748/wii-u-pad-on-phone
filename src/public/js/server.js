const socket = io();
const video = document.getElementById("vod");
const roomName = "upad";
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
// Display Media Functions
async function getMedia() {

  try {

    myStream = await navigator.mediaDevices.getDisplayMedia(displayMediaOptions);
    video.srcObject = myStream;
  } catch (error) {
    console.error(error);
  }
}

async function handleDisplayChange(event) {
  event.preventDefault();

  await getMedia();
  if (myPeerConnection) {
    const videoTrack = myStream.getVideoTracks()[0];
    const videoSender = myPeerConnection.getSenders().find((sender) => sender.track.kind === "video");
    videoSender.replaceTrack(videoTrack);
  }
}

// Event Listeners
selectBtn.addEventListener("click", handleDisplayChange);

// Welcome Form and Socket Code
async function initCall() {
  await getMedia();
  await makeConnection();
}

async function handleWelcomeSubmit() {
  await initCall();
  socket.emit("join_room", roomName);
}

// Socket Code
socket.on("welcome", handleWelcome);

async function handleWelcome() {
  const offer = await myPeerConnection.createOffer();
  myPeerConnection.setLocalDescription(offer);
  console.log("sent the offer");

  socket.emit("offer", offer, roomName);
}

socket.on("offer", handleOffer);

async function handleOffer(offer) {
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

// RTC Code
function makeConnection() {
  myPeerConnection = new RTCPeerConnection({
    iceServers: [
      "stun:stun.l.google.com:19302",
      "stun:stun1.l.google.com:19302",
      "stun:stun2.l.google.com:19302",
      "stun:stun3.l.google.com:19302",
      "stun:stun4.l.google.com:19302",
    ].map((url) => ({ urls: [url] })),
  });

  myPeerConnection.addEventListener("icecandidate", handleIce);
  myPeerConnection.addEventListener("addstream", handleAddStream);

  myStream.getTracks().forEach((track) => myPeerConnection.addTrack(track, myStream));

  // const encodingParameters = {
  //   resolutionScale: 1.0,  // 해상도 비율 설정
  //   framerateScale: 1.0,  // 프레임 속도 비율 설정
  //   bitrateScale: 1.0,    // 비트레이트 비율 설정
  // };

  // const videoSender = myPeerConnection.getSenders().find((sender) => sender.track.kind === "video");
  // videoSender.setParameters({ encodings: [encodingParameters] });

}

function handleAddStream(data) {
  console.log("received the stream");
}

// Start the process
handleWelcomeSubmit();
