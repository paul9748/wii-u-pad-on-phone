var socket = new WebSocket(`ws://${window.location.host}`);



const messageList = document.querySelector('ul');
const messageForm = document.querySelector('form');

socket.addEventListener('open',()=>{
    console.log('connecting to server');
});

socket.addEventListener('message',(message)=>{
    // console.log('received message:',message.data);
    const li = document.createElement('li');
    li.innerText = message.data;
    messageList.append(li);
});

socket.addEventListener('close',()=>{console.log('disconnecting from server');});

function handlesubmit(event){
    event.preventDefault();
    const input = messageForm.querySelector('input');
    console.log(input.value);
    socket.send(input.value);
    input.value = '';
};

messageForm.addEventListener('submit',handlesubmit);

