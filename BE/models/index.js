const fs = require('fs');
const path = require('path');
const Sequelize = require('sequelize');
const sequelize = require('../config/db');

const db = {};

function readDirSync(dir) {
    fs.readdirSync(dir).forEach(file => {
        const filePath = path.join(dir, file);
        if (fs.statSync(filePath).isDirectory()) {
        readDirSync(filePath);
        } else if (file.slice(-3) === '.js') {
        const modelDef = require(filePath);
        if (typeof modelDef === 'function') {
            const model = modelDef(sequelize, Sequelize.DataTypes);
            db[model.name] = model;
        }
        }
    });
}

readDirSync(__dirname);

Object.keys(db).forEach(modelName => {
    if (db[modelName].associate) {
        db[modelName].associate(db);
    }
});

db.sequelize = sequelize;
db.Sequelize = Sequelize;

module.exports = db;
