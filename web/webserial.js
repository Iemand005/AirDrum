
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

button.addEventListener("click", () => {
  
  navigator.serial
    .requestPort({ filters: [{ usbVendorId }] })
    .then(async port => {
      await port.open({ baudRate });

      while (port.readable) {
        const lineStream = port.readable.pipeThrough(new TextDecoderStream()).pipeThrough(new TransformStream(new TextLineStreamTransformer()));

        const reader = lineStream.getReader();

        try {
          while (true) {
            const { value, done } = await reader.read();
            if (done) {
              break;
            }
            
            const trimmedLine = value.trim();
            if (trimmedLine.length > 0) {
              parseData(trimmedLine);
            }
          }
        } catch (error) {
          console.error("Read error:", error);
        } finally {
          reader.releaseLock();
        }
      }
    }).catch((e) => console.warn("User did not select a port or something", e));
});