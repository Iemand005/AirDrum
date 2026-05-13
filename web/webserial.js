
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

async function startReadingLines(port) {
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

function uh() {
  navigator.serial.requestPort({ filters: [{ usbVendorId }] }).then(port => port.open({ baudRate }).then(startReadingLines)).catch(e => console.warn("User did not select a port or something", e));
}

function startListening() {
  uh(data => {
    console.log("Receied line:", data);
  })
}

button.addEventListener("click", startListening);