const db = require('../models');

exports.checkUserExists = async (username, password) => {
    const user = await db.User.findOne({ where: { username, password } });
    return user !== null;
};
