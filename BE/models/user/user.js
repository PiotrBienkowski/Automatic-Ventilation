module.exports = (sequelize, DataTypes) => {
    const User = sequelize.define('User', {
    username: {
        type: DataTypes.STRING,
        allowNull: false,
    },
    password: {
        type: DataTypes.STRING,
        unique: true,
    },
    });

    return User;
};
