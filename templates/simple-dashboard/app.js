const commandPreview = document.querySelector("#commandPreview");
const stateInput = document.querySelector("#stateInput");
const statePreview = document.querySelector("#statePreview");

const topic = "comfortfan/qihang01/cmd";

function renderCommand(command) {
  commandPreview.textContent =
    `mosquitto_pub -h 127.0.0.1 -p 1883 -t "${topic}" -m "${command}"`;
}

function renderState() {
  try {
    const state = JSON.parse(stateInput.value);
    statePreview.textContent = [
      `设备：${state.device ?? "--"}`,
      `温度：${state.temperature ?? "--"} °C`,
      `湿度：${state.humidity ?? "--"} %RH`,
      `模式：${state.mode ?? "--"}`,
      `虚拟风速：${state.virtual_speed ?? "--"} %`,
      `档位：${state.level ?? "--"}`,
      `ECO：${Number(state.eco) === 1 ? "ON" : "OFF"}`,
      `Motor Hold：${Number(state.motor_hold) === 1 ? "ON" : "OFF"}`,
      `Wi-Fi/MQTT：W:${state.wifi ?? 0} M:${state.mqtt ?? 0}`,
    ].join("\n");
  } catch (error) {
    statePreview.textContent = `JSON 解析失败：${error.message}`;
  }
}

for (const button of document.querySelectorAll("[data-command]")) {
  button.addEventListener("click", () => renderCommand(button.dataset.command));
}

stateInput.addEventListener("input", renderState);
renderCommand("STATUS");
renderState();
