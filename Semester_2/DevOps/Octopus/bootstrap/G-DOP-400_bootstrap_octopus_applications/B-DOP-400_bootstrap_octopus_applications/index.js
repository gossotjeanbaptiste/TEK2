
const db = require('./db');
const config = require('./config.json')

const express = require('express');
const app = express();

app.get('/', (req, res) => {
    db.ping_redis()
        .then((health) => {
            res.send({
                redis: !!health ? "healthy": "KO",
            });
        });
});

app.listen(config['port'], () => console.log(`Example app listening`));
