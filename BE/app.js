const express = require('express');
const app = express();
const port = 3002;

app.use(express.json());

app.get('/', (req, res) => {
    const currentDate = new Date();
    res.json({ date: currentDate });
});

app.listen(port, () => {
    console.log(`API works on http://localhost:${port}`);
});
