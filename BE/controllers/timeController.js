const moment = require('moment-timezone');

exports.isTimeAllowed = () => {
    const now = moment().tz('Europe/Warsaw');
    const currentTime = now.format('HH:mm');
    const startTime = '07:20';
    const endTime = '23:30';
  
    return (currentTime >= startTime && currentTime < endTime);
};