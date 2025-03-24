import express from 'express';
import { WebSocketServer } from 'ws';
import { createServer } from 'http';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const server = createServer(app);
const wss = new WebSocketServer({ server });

// Serve static files from public directory
app.use(express.static(join(__dirname, 'public')));

// Simulated system state
let systemState = {
  temperature: 22,
  humidity: 45,
  motion: false,
  light: 500,
  airQuality: 95,
  doorState: 'LOCKED',
  windowOpening: 0,
  fanSpeed: 'OFF',
  lightLevel: 0
};

// Simulate sensor data with more realistic variations
function generateSensorData() {
  // Add small random variations to make data more realistic
  systemState.temperature += (Math.random() - 0.5) * 0.2;
  systemState.humidity += (Math.random() - 0.5) * 0.5;
  systemState.motion = Math.random() > 0.8;
  systemState.light += (Math.random() - 0.5) * 10;
  systemState.airQuality += (Math.random() - 0.5) * 0.2;

  // Keep values within realistic ranges
  systemState.temperature = Math.max(15, Math.min(35, systemState.temperature));
  systemState.humidity = Math.max(30, Math.min(70, systemState.humidity));
  systemState.light = Math.max(0, Math.min(1000, systemState.light));
  systemState.airQuality = Math.max(0, Math.min(100, systemState.airQuality));

  return {
    ...systemState,
    timestamp: new Date().toISOString()
  };
}

// Handle WebSocket connections
wss.on('connection', (ws) => {
  console.log('Client connected');

  // Send initial state
  ws.send(JSON.stringify(generateSensorData()));

  // Set up regular updates
  const interval = setInterval(() => {
    ws.send(JSON.stringify(generateSensorData()));
  }, 2000);

  // Handle control commands from client
  ws.on('message', (message) => {
    try {
      const command = JSON.parse(message);
      handleCommand(command);
      // Send immediate update after command
      ws.send(JSON.stringify(generateSensorData()));
    } catch (error) {
      console.error('Error handling message:', error);
    }
  });

  ws.on('close', () => {
    console.log('Client disconnected');
    clearInterval(interval);
  });
});

// Handle system commands
function handleCommand(command) {
  switch (command.type) {
    case 'SET_TEMPERATURE':
      systemState.temperature = command.value;
      break;
    case 'SET_FAN':
      systemState.fanSpeed = command.value;
      break;
    case 'SET_LIGHT':
      systemState.lightLevel = command.value;
      break;
    case 'SET_WINDOW':
      systemState.windowOpening = command.value;
      break;
    case 'SET_DOOR':
      systemState.doorState = command.value;
      break;
  }
}

// Start server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Smart Home Automation System running at http://localhost:${PORT}`);
  console.log('WebSocket server is ready for connections');
});