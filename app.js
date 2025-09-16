const http = require('http');
const { join } = require('node:path');
const express = require('express');
const app = express();
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const { Server } = require('socket.io'); 
const server = http.createServer(app);
const io = new Server(server);

//Serialport setup
const parser = new ReadlineParser({
    delimiter: "\r\n"
});

const port = new SerialPort({ path:'COM5', 
    baudRate: 9600
});

port.pipe(parser);

app.get('/', (req, res) => { res.sendFile(join(__dirname, 'index.html')); });

io.on('connection', (socket) => {
    console.log('Connection Established');
    
    socket.on('form', (obj) => {
        console.log(obj);
        port.write('<' + obj.co2 + ', ' + obj.h2 +', ' + obj.purge + ', ' + obj.cycles + '>');
    });

});

parser.on('data', (data) => {
    
    if (data.charAt(0) === '&') {
        io.emit('end', data.slice(1)); console.log(data.slice(1)); }
    else {
        io.emit('data', data); console.log(data); }
    
    });

server.listen(3000, () => { console.log('Connection Open on *:3000'); });
