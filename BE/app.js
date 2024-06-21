const express = require('express');
const db = require('./models');
const os = require('os');
const timeController = require('./controllers/timeController');
const temperatureController = require('./controllers/temperatureController');


const app = express();
const port = 3002;

app.use(express.json());

db.sequelize.sync({ force: false })
.then(() => {
    console.log('The models have been synchronized with the database.');
})
.catch(err => {
    console.error('Error synchronizing models with the database:', err);
});

const getLocalIPAddress = () => {
    const interfaces = os.networkInterfaces();
    for (const name of Object.keys(interfaces)) {
      for (const net of interfaces[name]) {
        const { family, internal, address } = net;
        if (family === 'IPv4' && !internal) {
          return address;
        }
      }
    }
    return 'localhost';
  };

app.get('/ping', (req, res) => {
    const currentDate = new Date();
    res.json({ date: currentDate });
});

app.get('/users', async (req, res) => {
    try {
        const users = await db.User.findAll();
        res.json(users);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/temperatures', async (req, res) => {
    try {
        const temperatures = await temperatureController.getAllTemperatures();
        res.json(temperatures);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.post('/temperatures', async (req, res) => {
    try {
        const { username, password, inside, outside } = req.body;
        const temperature = await temperatureController.addTemperature(username, password, inside, outside);
        res.status(201).json(temperature);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/check-time', (req, res) => {
    const allowed = timeController.isTimeAllowed();
    res.json({ allowed });
});

app.listen(port, '0.0.0.0', () => {
    const localIP = getLocalIPAddress();
    console.log(`API działa na http://${localIP}:${port}`);
});