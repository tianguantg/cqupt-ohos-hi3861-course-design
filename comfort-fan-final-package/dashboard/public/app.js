const $ = (selector) => document.querySelector(selector);
const buttons = [...document.querySelectorAll("[data-command]")];
const events = $("#events");

let lastRecentKey = "";
let previousState = null;
let displayedSpeed = null;
let speedAnimation = null;

function value(text, fallback = "--") {
  return text === undefined || text === null || text === "" ? fallback : text;
}

function setLamp(id, active) {
  const lamp = $(id);
  lamp.classList.toggle("on", Boolean(active));
  lamp.classList.toggle("off", !active);
}

function appendLocal(kind, text) {
  const item = document.createElement("li");
  item.className = kind;
  item.innerHTML = `<time>${new Date().toLocaleTimeString()}</time><span>${text}</span>`;
  events.prepend(item);
  while (events.children.length > 60) {
    events.lastElementChild.remove();
  }
}

function renderRecent(recent) {
  if (!recent?.length) return;
  const key = `${recent[0].at}:${recent[0].kind}:${recent[0].text}`;
  if (key === lastRecentKey) return;
  lastRecentKey = key;

  events.replaceChildren();
  for (const entry of recent.slice(0, 28)) {
    const item = document.createElement("li");
    item.className = entry.kind;
    const at = new Date(entry.at).toLocaleTimeString();
    item.innerHTML = `<time>${at}</time><span>${entry.text}</span>`;
    events.append(item);
  }
}

function targetFromTemperature(temperature) {
  if (!Number.isFinite(temperature)) return { base: 0, label: "温度数据暂不可用" };
  if (temperature < 26) return { base: 0, label: "低于 26°C，基础目标为 0%" };
  if (temperature < 27.5) return { base: 30, label: "26-27.4°C，基础目标为 30%" };
  if (temperature < 29) return { base: 45, label: "27.5-28.9°C，基础目标为 45%" };
  if (temperature < 31) return { base: 60, label: "29-30.9°C，基础目标为 60%" };
  return { base: 75, label: "达到 31°C 以上，基础目标为 75%" };
}

function humidityBoost(humidity) {
  if (!Number.isFinite(humidity)) return { boost: 0, label: "湿度数据暂不可用" };
  if (humidity > 75) return { boost: 10, label: "湿度超过 75%，额外增加 10%" };
  if (humidity > 65) return { boost: 5, label: "湿度超过 65%，额外增加 5%" };
  return { boost: 0, label: "湿度未超过补偿阈值" };
}

function clampSpeed(speed) {
  return Math.max(0, Math.min(100, speed));
}

function animateNumber(element, nextValue, options = {}) {
  const { duration = 700, suffix = "", decimals = 0 } = options;
  const target = Number(nextValue);

  if (!Number.isFinite(target)) {
    element.textContent = value(nextValue);
    return;
  }

  const start = Number.isFinite(displayedSpeed) ? displayedSpeed : target;
  const startTime = performance.now();

  if (speedAnimation) {
    cancelAnimationFrame(speedAnimation);
  }

  function tick(now) {
    const progress = Math.min(1, (now - startTime) / duration);
    const eased = 1 - Math.pow(1 - progress, 3);
    const current = start + (target - start) * eased;
    displayedSpeed = current;
    element.textContent = `${current.toFixed(decimals)}${suffix}`;
    if (progress < 1) {
      speedAnimation = requestAnimationFrame(tick);
    } else {
      displayedSpeed = target;
      speedAnimation = null;
    }
  }

  speedAnimation = requestAnimationFrame(tick);
}

function explainState(state, previous) {
  if (!state || state.sensor_ok === 0) {
    return {
      title: "等待有效传感器数据",
      text: "温湿度数据暂不可用，控制台会先保持最后状态。",
    };
  }

  const mode = String(state.mode || "OFF");
  const speed = Number(state.virtual_speed);
  const lastSpeed = previous ? Number(previous.virtual_speed) : NaN;
  const eco = Number(state.eco) === 1;
  const preference = Number.isFinite(state.preference) ? Number(state.preference) : (eco ? -10 : 0);
  const temperature = Number(state.temperature);
  const humidity = Number(state.humidity);
  const temp = targetFromTemperature(temperature);
  const humid = humidityBoost(humidity);
  const quietTarget = eco ? 15 : 25;
  const autoTarget = clampSpeed(temp.base + humid.boost + preference);
  const trend = Number.isFinite(lastSpeed) && Number.isFinite(speed)
    ? speed > lastSpeed ? "up" : speed < lastSpeed ? "down" : "same"
    : "same";

  if (mode === "OFF") {
    return {
      title: "风扇已关闭",
      text: "当前模式为 OFF，目标风速为 0%，电机停止。",
    };
  }

  if (mode === "QUIET") {
    const trendText = trend === "up" ? "正在平滑升到安静档" : trend === "down" ? "正在平滑降到安静档" : "已接近安静档目标";
    return {
      title: `安静模式目标 ${quietTarget}%`,
      text: `识别到 QUIET 模式，${eco ? "ECO 已开启，目标从 25% 降为 15%" : "目标风速为 25%"}。${trendText}，每次最多变化 5%。`,
    };
  }

  const reason = [`识别到 ${temperature.toFixed(1)}°C，${temp.label}`];
  if (humid.boost > 0) {
    reason.push(`${humidity.toFixed(1)}%RH，${humid.label}`);
  } else {
    reason.push(`${humidity.toFixed(1)}%RH，暂不做湿度补偿`);
  }
  if (preference < 0) {
    reason.push(`ECO 偏置 ${preference}%，降低目标以省电`);
  } else if (preference > 0) {
    reason.push(`舒适偏置 +${preference}%，提高目标风速`);
  }

  let action = "目标没有变化，风速保持稳定";
  if (trend === "up") {
    action = "正在增加风扇转速，让体感更凉爽";
  } else if (trend === "down") {
    action = "正在降低风扇转速，减少不必要的吹风";
  } else if (Number.isFinite(speed) && speed !== autoTarget) {
    action = "正在等待下一次平滑调整，每次最多变化 5%";
  }

  return {
    title: `AUTO 目标 ${autoTarget}%`,
    text: `${reason.join("；")}。${action}。`,
  };
}

function render(payload) {
  const state = payload.lastState || {};
  const wifi = Number(state.wifi) === 1;
  const mqtt = Number(state.mqtt) === 1;

  setLamp("#brokerLamp", payload.mqttOnline);
  setLamp("#deviceLamp", wifi && mqtt);
  $("#brokerText").textContent = payload.mqttOnline ? "Broker online" : "Broker offline";
  $("#deviceText").textContent = wifi && mqtt ? "W:1 M:1" : `W:${wifi ? 1 : 0} M:${mqtt ? 1 : 0}`;

  $("#temperature").textContent = Number.isFinite(state.temperature) ? state.temperature.toFixed(1) : "--.-";
  $("#humidity").textContent = Number.isFinite(state.humidity) ? state.humidity.toFixed(1) : "--.-";
  animateNumber($("#speed"), state.virtual_speed);
  $("#mode").textContent = value(state.mode);
  $("#level").textContent = value(state.level);
  $("#eco").textContent = Number(state.eco) === 1 ? "ON" : "OFF";
  $("#motorHold").textContent = Number(state.motor_hold) === 1 ? "ON" : "OFF";

  const insight = explainState(state, previousState);
  $("#insightTitle").textContent = insight.title;
  $("#insightText").textContent = insight.text;

  $("#mqttUrl").textContent = payload.mqttUrl || "mqtt://--";
  $("#topics").textContent = payload.topics ? `${payload.topics.state} / ${payload.topics.cmd}` : "comfortfan/--";

  document.body.dataset.mode = String(state.mode || "OFF").toLowerCase();
  renderRecent(payload.recent);
  previousState = state;
}

async function send(command) {
  appendLocal("pending", `TX ${command}`);
  for (const button of buttons) button.disabled = true;
  try {
    const response = await fetch("/api/command", {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify({ command }),
    });
    const data = await response.json();
    if (!response.ok || !data.ok) {
      throw new Error(data.error || "Command failed");
    }
  } catch (error) {
    appendLocal("error", error.message);
  } finally {
    setTimeout(() => {
      for (const button of buttons) button.disabled = false;
    }, 250);
  }
}

for (const button of buttons) {
  button.addEventListener("click", () => send(button.dataset.command));
}

$("#clearLog").addEventListener("click", () => {
  events.replaceChildren();
  lastRecentKey = "";
});

function connectLive() {
  const socket = new WebSocket(`${location.protocol === "https:" ? "wss" : "ws"}://${location.host}/live`);
  socket.addEventListener("message", (event) => render(JSON.parse(event.data)));
  socket.addEventListener("close", () => {
    setLamp("#brokerLamp", false);
    $("#brokerText").textContent = "UI reconnecting";
    setTimeout(connectLive, 1200);
  });
}

connectLive();
