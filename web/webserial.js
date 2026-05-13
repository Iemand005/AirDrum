
const usbVendorId = 0x0403;

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
      await port.open({ baudRate: 9600 });

      while (port.readable) {
        const reader = port.readable.getReader();
        try {
          while (true) {
            const { value, done } = await reader.read();
            if (done) {
              // |reader| has been canceled.
              break;
            }
            // Do something with |value|...
            // console.log("", value);

            const data = new TextDecoder().decode(value);
            console.log(data);
          }
        } catch (error) {
          // Handle |error|...
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