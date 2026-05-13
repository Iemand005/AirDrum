
const usbVendorId = 0x0403;

const baudRate = 115200;

const button = document.getElementById("button");

navigator.serial.addEventListener("connect", (e) => {
  console.log("Connected", e);
});

navigator.serial.addEventListener("disconnect", (e) => {
  console.log("disconn", e)
});

navigator.serial.getPorts().then((ports) => {
  console.log("gor ports", ports)
});

async function startReadingLines(port, callback) {

  console.log("port reached here!", port)
  let buffer = "";

  while (port.readable) {
    const textDecoderStream = new TextDecoderStream();
    port.readable.pipeTo(textDecoderStream.writable);
    const reader = textDecoderStream.readable.getReader();

    try {
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;

        buffer += value;
        const lines = buffer.split("\n");
        buffer = lines.pop();

        for (const line of lines) {
          callback(line);
        }
      }
    } catch (error) {
      console.error(error);
    } finally {
      reader.releaseLock();
    }
  }
}

function uh(callback) {
  navigator.serial.requestPort({ filters: [{ usbVendorId }] }).then(port => port.open({ baudRate }).then(_ => startReadingLines(port, callback))).catch(e => console.warn("User did not select a port or something", e));
}

let lastCode = 0;

function startListening() {
  uh(data => {
    console.log("Receied line:", data);
    // ok i wanna now check if it's a drum sound and play
    var splits = data.split(" ");
    if (splits[0] == "Received") console.log("This is a received packet");
    else return;
    const code = splits[2];
    if (code == lastCode) return console.log("Skippfk");
    lastCode = code;
    const val = splits[4];

    console.log("code!!!: ", code, val);

    if (val === "67") playBeep(2000);
    if (val === "69") playBeep(1000);
    
  })
}

button.addEventListener("click", startListening);