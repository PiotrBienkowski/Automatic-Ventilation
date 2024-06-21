const express = require('express');
const db = require('./models');

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
        const temperatures = await db.Temperature.findAll();
        res.json(temperatures);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});
  

app.listen(port, () => {
    console.log(`API działa na http://localhost:${port}`);
});
