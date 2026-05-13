
const usbVendorId = 0x0403;

const baudRate = 115200;

const button = document.getElementById("button");

navigator.serial.addEventListener("connect", (e) => {
  // Connect to `e.target` or add it to a list of available ports.
  console.log("Connected", e);
  // console.log(e);
});

navigator.serial.addEventListener("disconnect", (e) => {
  console.log("disconn", e)
  // Remove `e.target` from the list of available ports.
});

navigator.serial.getPorts().then((ports) => {
  console.log("gor ports", ports)
  // Initialize the list of available ports with `ports` on page load.
});

button.addEventListener("click", () => {
  
  navigator.serial
    .requestPort({ filters: [{ usbVendorId }] })
    .then(async port => {
      // port
      await port.open({ baudRate });

      while (port.readable) {
        const lineStream = port.readable
          .pipeThrough(new TextDecoderStream())
          .pipeThrough(new TransformStream(new TextLineStreamTransformer()));

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
      // Connect to `port` or add it to the list of available ports.
    })
    .catch((e) => {
      // The user didn't select a port.
    });
});