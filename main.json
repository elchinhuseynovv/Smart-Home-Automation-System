import express from 'express';
import { WebSocketServer } from 'ws';
import { createServer } from 'http';

const app = express();
const server = createServer(app);
const wss = new WebSocketServer({ server });

// Serve static files
app.use(express.static('public'));

// Simulate sensor data
function generateSensorData() {
  return {
    temperature: 20 + Math.random() * 10,
    humidity: 40 + Math.random() * 20,
    motion: Math.random() > 0.5,
    light: Math.floor(Math.random() * 1000),
    airQuality: 80 + Math.random() * 20,
    timestamp: new Date().toISOString()
  };
}

// WebSocket connection handling
wss.on('connection', (ws) => {
  console.log('Client connected');

  // Send sensor data every 2 seconds
  const interval = setInterval(() => {
    const data = generateSensorData();
    ws.send(JSON.stringify(data));
  }, 2000);

  ws.on('close', () => {
    console.log('Client disconnected');
    clearInterval(interval);
  });
});

// Start server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Smart Home Automation System running at http://localhost:${PORT}`);
  console.log('WebSocket server is ready for connections');
});