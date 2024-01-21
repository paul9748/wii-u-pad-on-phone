const socket = io();
const view = document.getElementById("myface");
const muteBtn = document.getElementById("mute");
const videoBtn = document.getElementById("video");

let myStream;

let muted = false;
let videoOff = false;
async function getMedia() {
  try {
    myStream = await navigator.mediaDevices.getUserMedia({
      audio: true,
      video: true,
    });
    console.log(myStream);
    myface.srcObject = myStream;
  } catch (e) {}
}

function handleMuteClick(event) {
  //   myStream
  //     .getAudioTracks()
  //     .forEach((track) => (track.enabled = !track.enabled));

  if (!muted) {
    muteBtn.innerText = "Muted";
  } else {
    muteBtn.innerText = "Mute";
  }
  muted = !muted;
}

function handleVideoClick(event) {
  myStream
    .getVideoTracks()
    .forEach((track) => (track.enabled = !track.enabled));

  if (!videoOff) {
    videoBtn.innerText = "Video On";
  } else {
    videoBtn.innerText = "Video Off";
  }
  videoOff = !videoOff;
}

muteBtn.addEventListener("click", handleMuteClick);
videoBtn.addEventListener("click", handleVideoClick);

getMedia();
