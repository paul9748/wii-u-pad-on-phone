var socket = new WebSocket(`ws://${window.location.host}`);



const messageList = document.querySelector('ul');
const nickForm = document.querySelector("#nick");
const messageForm = document.querySelector("#message");


function makeMessage(type, payload) {
    const msg = { type, payload };
    return JSON.stringify(msg);
}

socket.addEventListener('open', () => {
    console.log('connecting to server');
});

socket.addEventListener('message', (message) => {
    // console.log('received message:',message.data);
    const li = document.createElement('li');
    li.innerText = message.data;
    messageList.append(li);
});

socket.addEventListener('close', () => { console.log('disconnecting from server'); });

function handleSubmit(event) {
    event.preventDefault();
    const input = messageForm.querySelector('input');
    console.log(input.value);
    socket.send(makeMessage("new_message", input.value));
    const li = document.createElement('li');
    li.innerText = `You:${input.value}`;
    messageList.append(li);
    input.value = '';
};
function handleNickSubmit(event) {
    event.preventDefault();
    const input = nickForm.querySelector("input");
    socket.send(makeMessage("nickname", input.value));
}

messageForm.addEventListener("submit", handleSubmit);
nickForm.addEventListener("submit", handleNickSubmit);

