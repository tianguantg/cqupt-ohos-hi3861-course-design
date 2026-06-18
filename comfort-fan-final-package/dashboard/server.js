import express from "express";
import { createServer } from "node:http";
import os from "node:os";
import mqtt from "mqtt";
import { WebSocketServer } from "ws";

const PORT = Number(process.env.PORT || 8787);
const HOST = process.env.HOST || "0.0.0.0";
const MQTT_URL = process.env.MQTT_URL || "mqtt://192.168.137.1:1883";
const DEVICE_ID = process.env.DEVICE_ID || "qihang01";

const topics = {
  state: `comfortfan/${DEVICE_ID}/state`,
  cmd: `comfortfan/${DEVICE_ID}/cmd`,
  event: `comfortfan/${DEVICE_ID}/event`,
};

const app = express();
const server = createServer(app);
const wss = new WebSocketServer({ server, path: "/live" });

let mqttOnline = false;
let lastState = null;
let lastEvent = null;
let lastCommand = null;
const recent = [];

app.use(express.static("public"));
app.use(express.json());

function remember(entry) {
  recent.unshift({ at: new Date().toISOString(), ...entry });
  recent.splice(80);
}

function sendTo(socket, payload) {
  if (socket.readyState === socket.OPEN) {
    socket.send(JSON.stringify(payload));
  }
}

function broadcast(payload) {
  for (const socket of wss.clients) {
    sendTo(socket, payload);
  }
}

function snapshot() {
  return {
    type: "snapshot",
    mqttOnline,
    mqttUrl: MQTT_URL,
    deviceId: DEVICE_ID,
    topics,
    lastState,
    lastEvent,
    lastCommand,
    recent,
  };
}

const client = mqtt.connect(MQTT_URL, {
  clientId: `comfort-fan-ui-${Math.random().toString(16).slice(2)}`,
  clean: true,
  connectTimeout: 5000,
  keepalive: 30,
  reconnectPeriod: 2000,
});

client.on("connect", () => {
  mqttOnline = true;
  remember({ kind: "broker", text: "MQTT broker connected" });
  client.subscribe([topics.state, topics.event], { qos: 0 });
  broadcast(snapshot());
});

client.on("reconnect", () => {
  remember({ kind: "broker", text: "Reconnecting to MQTT broker" });
  broadcast(snapshot());
});

client.on("close", () => {
  mqttOnline = false;
  broadcast(snapshot());
});

client.on("error", (error) => {
  remember({ kind: "error", text: error.message });
  broadcast(snapshot());
});

client.on("message", (topic, buffer) => {
  const text = buffer.toString();
  if (topic === topics.state) {
    try {
      lastState = JSON.parse(text);
    } catch {
      lastState = { raw: text };
    }
    remember({ kind: "state", text });
  } else if (topic === topics.event) {
    lastEvent = text;
    remember({ kind: "event", text });
  }
  broadcast(snapshot());
});

app.post("/api/command", (request, response) => {
  const command = String(request.body?.command || "").trim();
  const allowed = new Set([
    "OFF",
    "QUIET",
    "AUTO",
    "ECO_ON",
    "ECO_OFF",
    "MORE",
    "LESS",
    "MOTOR_ON",
    "MOTOR_OFF",
    "STATUS",
  ]);

  if (!allowed.has(command)) {
    response.status(400).json({ ok: false, error: "Unsupported command" });
    return;
  }

  client.publish(topics.cmd, command, { qos: 0, retain: false }, (error) => {
    if (error) {
      response.status(502).json({ ok: false, error: error.message });
      return;
    }
    lastCommand = { command, at: new Date().toISOString() };
    remember({ kind: "command", text: command });
    broadcast(snapshot());
    response.json({ ok: true });
  });
});

wss.on("connection", (socket) => {
  sendTo(socket, snapshot());
});

function localUrls() {
  const urls = [`http://127.0.0.1:${PORT}`];
  for (const addresses of Object.values(os.networkInterfaces())) {
    for (const address of addresses || []) {
      if (address.family === "IPv4" && !address.internal) {
        urls.push(`http://${address.address}:${PORT}`);
      }
    }
  }
  return urls;
}

server.listen(PORT, HOST, () => {
  console.log("Comfort fan dashboard:");
  for (const url of localUrls()) {
    console.log(`  ${url}`);
  }
  console.log(`MQTT broker: ${MQTT_URL}`);
  console.log(`Device: ${DEVICE_ID}`);
});
