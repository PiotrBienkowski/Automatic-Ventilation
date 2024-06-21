const db = require('../models');
const userController = require('./userController');

exports.addTemperature = async (username, password, inside, outside) => {
    const userExists = await userController.checkUserExists(username, password);
    if (userExists) {
        const temperature = await db.Temperature.create({ inside, outside });
        return temperature;
    } else {
        throw new Error('The user does not exist.');
    }
};

exports.getAllTemperatures = async () => {
    const temperatures = await db.Temperature.findAll();
    return temperatures;
};
