module.exports = (sequelize, DataTypes) => {
    const Temperature = sequelize.define('Temperature', {
    inside: {
        type: DataTypes.FLOAT,
        allowNull: false,
    },
    outside: {
        type: DataTypes.FLOAT,
        allowNull: false,
    },
    });

    return Temperature;
};
