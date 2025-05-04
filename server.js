// === server.js ===
const WebSocket = require('ws');
const PORT = process.env.PORT || 8080;
const server = new WebSocket.Server({ port: PORT });

let clients = [];

server.on('connection', (ws) => {
  console.log('Client connected');
  clients.push(ws);

  ws.on('message', (message) => {
    console.log('Received:', message);

    // Broadcast to all other clients
    clients.forEach(client => {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
  });

  ws.on('close', () => {
    console.log('Client disconnected');
    clients = clients.filter(c => c !== ws);
  });
});

console.log(`WebSocket server running on port ${PORT}`);
