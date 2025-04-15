import express from 'express';
import { WebSocketServer } from 'ws';
import { createServer } from 'http';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import cors from 'cors';
import dotenv from 'dotenv';
import moment from 'moment';
import { NlpManager } from 'node-nlp';
import axios from 'axios';
import { v4 as uuidv4 } from 'uuid';
import Chart from 'chart.js/auto';

dotenv.config();

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static(join(__dirname, 'public')));

const server = createServer(app);
const wss = new WebSocketServer({ server });

// Initialize NLP manager for natural language processing
const nlpManager = new NlpManager({ languages: ['en'] });

// Enhanced system state with more features
let systemState = {
  temperature: 22,
  humidity: 45,
  motion: false,
  light: 500,
  airQuality: 95,
  doorState: 'LOCKED',
  windowOpening: 0,
  fanSpeed: 'OFF',
  lightLevel: 0,
  energyConsumption: 0,
  solarProduction: 0,
  batteryLevel: 100,
  securityStatus: 'ARMED',
  occupancy: false,
  weatherForecast: {},
  automationRules: [],
  deviceSchedules: {},
  notifications: [],
  scenes: {},
  maintenanceSchedule: [],
  deviceHealth: {},
  userPreferences: {}
};

// Enhanced analytics tracking
const analytics = {
  energyUsage: [],
  temperature: [],
  humidity: [],
  occupancy: [],
  events: []
};

// Simulate more realistic sensor data with advanced patterns
function generateSensorData() {
  const time = moment();
  const hour = time.hour();
  
  // Time-based temperature variations
  const baseTemp = 22;
  const tempVariation = Math.sin(hour / 24 * 2 * Math.PI) * 3;
  systemState.temperature = baseTemp + tempVariation + (Math.random() - 0.5);
  
  // Occupancy-based humidity changes
  if (systemState.occupancy) {
    systemState.humidity += (Math.random() - 0.4) * 0.5;
  } else {
    systemState.humidity += (Math.random() - 0.6) * 0.3;
  }
  
  // Smart motion detection
  systemState.motion = Math.random() > (systemState.occupancy ? 0.7 : 0.95);
  
  // Dynamic light adjustment based on time
  const isDaytime = hour >= 6 && hour <= 18;
  systemState.light = isDaytime ? 
    500 + Math.random() * 300 : 
    50 + Math.random() * 100;
  
  // Air quality degradation when occupied
  if (systemState.occupancy) {
    systemState.airQuality -= 0.1;
  } else {
    systemState.airQuality += 0.05;
  }
  
  // Energy consumption patterns
  systemState.energyConsumption = calculateEnergyConsumption();
  systemState.solarProduction = calculateSolarProduction(hour);
  
  // Keep values within realistic ranges
  systemState.temperature = Math.max(15, Math.min(35, systemState.temperature));
  systemState.humidity = Math.max(30, Math.min(70, systemState.humidity));
  systemState.light = Math.max(0, Math.min(1000, systemState.light));
  systemState.airQuality = Math.max(0, Math.min(100, systemState.airQuality));
  systemState.batteryLevel = Math.max(0, Math.min(100, systemState.batteryLevel));
  
  // Update analytics
  updateAnalytics();
  
  return {
    ...systemState,
    timestamp: time.toISOString()
  };
}

// Enhanced energy consumption calculation
function calculateEnergyConsumption() {
  let consumption = 0;
  
  // Base load
  consumption += 500;
  
  // HVAC consumption
  if (Math.abs(systemState.temperature - 22) > 2) {
    consumption += 1000;
  }
  
  // Lighting consumption
  consumption += (systemState.lightLevel / 255) * 200;
  
  // Appliance consumption
  if (systemState.occupancy) {
    consumption += 300;
  }
  
  return consumption;
}

// Solar production calculation
function calculateSolarProduction(hour) {
  if (hour >= 6 && hour <= 18) {
    const peakHour = 12;
    const efficiency = 1 - Math.abs(hour - peakHour) / 6;
    return 2000 * efficiency * (0.8 + Math.random() * 0.4);
  }
  return 0;
}

// Enhanced analytics tracking
function updateAnalytics() {
  const timestamp = moment().toISOString();
  
  analytics.energyUsage.push({
    timestamp,
    consumption: systemState.energyConsumption,
    production: systemState.solarProduction
  });
  
  analytics.temperature.push({
    timestamp,
    value: systemState.temperature
  });
  
  analytics.humidity.push({
    timestamp,
    value: systemState.humidity
  });
  
  // Maintain data for last 24 hours only
  const dayAgo = moment().subtract(24, 'hours');
  analytics.energyUsage = analytics.energyUsage.filter(
    data => moment(data.timestamp).isAfter(dayAgo)
  );
}
