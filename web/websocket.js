
const button = document.getElementById("button");

button.onclick = () => {
  const socket = new WebSocket("ws://10.25.241.136/ws");

  socket.onopen = () => {
    console.log('CONNECTED TO AirDrumServer!');
    
    console.log('Testbericht sending: HIT_SNARE');
    socket.send('HIT_SNARE');
  };

  socket.onmessage = (event) => {
      console.log('Message receivd', event.data);
  };

  socket.onclose = () => {
      console.log('closedrt');
  };

  socket.onerror = (error) => {
      console.error('Werror gone shit', error);
  };
};