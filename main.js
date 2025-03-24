import express from 'express';
import { WebSocketServer } from 'ws';
import { createServer } from 'http';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import cors from 'cors';
import dotenv from 'dotenv';

dotenv.config();

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static(join(__dirname, 'public')));

const server = createServer(app);
const wss = new WebSocketServer({ server });

// Simulated system state with initial values
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

// Broadcast to all connected clients
function broadcast(data) {
  wss.clients.forEach(client => {
    if (client.readyState === 1) {
      client.send(JSON.stringify(data));
    }
  });
}

// Handle WebSocket connections
wss.on('connection', (ws) => {
  console.log('Client connected');

  // Send initial state
  ws.send(JSON.stringify(generateSensorData()));

  // Set up regular updates
  const interval = setInterval(() => {
    const data = generateSensorData();
    broadcast(data);
  }, 2000);

  // Handle control commands from client
  ws.on('message', (message) => {
    try {
      const command = JSON.parse(message.toString());
      handleCommand(command);
      // Send immediate update after command
      const data = generateSensorData();
      broadcast(data);
    } catch (error) {
      console.error('Error handling message:', error);
      ws.send(JSON.stringify({ error: 'Invalid command format' }));
    }
  });

  ws.on('error', (error) => {
    console.error('WebSocket error:', error);
  });

  ws.on('close', () => {
    console.log('Client disconnected');
    clearInterval(interval);
  });
});

// Handle system commands
function handleCommand(command) {
  console.log('Received command:', command);
  
  switch (command.type) {
    case 'SET_TEMPERATURE':
      if (typeof command.value === 'number' && command.value >= 15 && command.value <= 35) {
        systemState.temperature = command.value;
      }
      break;
    case 'SET_FAN':
      if (['OFF', 'LOW', 'MEDIUM', 'HIGH'].includes(command.value)) {
        systemState.fanSpeed = command.value;
      }
      break;
    case 'SET_LIGHT':
      if (typeof command.value === 'number' && command.value >= 0 && command.value <= 255) {
        systemState.lightLevel = command.value;
      }
      break;
    case 'SET_WINDOW':
      if (typeof command.value === 'number' && command.value >= 0 && command.value <= 100) {
        systemState.windowOpening = command.value;
      }
      break;
    case 'SET_DOOR':
      if (['LOCKED', 'UNLOCKED'].includes(command.value)) {
        systemState.doorState = command.value;
      }
      break;
    default:
      console.warn('Unknown command type:', command.type);
  }
}

// Error handling for the server
server.on('error', (error) => {
  console.error('Server error:', error);
});

// Start server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Smart Home Automation System running at http://localhost:${PORT}`);
  console.log('WebSocket server is ready for connections');
});

// Handle process termination
process.on('SIGTERM', () => {
  console.log('SIGTERM received. Closing server...');
  server.close(() => {
    console.log('Server closed');
    process.exit(0);
  });
});