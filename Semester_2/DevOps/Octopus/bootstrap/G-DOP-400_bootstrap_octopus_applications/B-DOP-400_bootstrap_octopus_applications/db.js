
const redis = require("redis");
const config = require('./config.json');

const client = redis.createClient({
    host: config['redis']['host'],
    retry_strategy: () => 1000,
});
client.on("end", () => {
    // nothing
});
client.on("error", function (err) {
    // nothing
});

const ping_redis = () => {
    const timeout = new Promise((resolve, reject) => {
        const id = setTimeout(() => {
            clearTimeout(id);
            resolve(false);
        }, 1000);
    });

    const query = new Promise((resolve, reject) => {
        client.keys('*', (err, data) => {
            if (!!err) resolve(false);
            else resolve(true);
        });
    });
    return Promise.race([timeout, query]);
};

module.exports = { ping_redis };
