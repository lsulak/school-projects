/*
 * Author:      Ladislav Sulak (laco.sulak@gmail.com)
 * Subject:     Assignment 1 in the subject Advanced Multimedia Technologies - TEI Crete.
 * Description: Web application using node.js and socket.io - encrypted chat, video, microphone
 *              and Tic Tac Toe game.
 */

// Function which handles responses, send files etc.
function handleRequest(request, response){
    console.log("Path Hit: " + request.url);
    if(request.url === '/'){
        fs.readFile(__dirname + "/index.html", function (err, data) {
            if (err) {
              response.writeHead(500);
              return response.end("Error loading index.html");
            }
            response.writeHead(200);
            response.end(data);
        });
    }
    else{
        fs.readFile(__dirname + request.url, function (err, data) {
        if (err) {
            response.writeHead(500);
            return response.end("Error in loading page");
        }
        response.writeHead(200);
        response.end(data);
        });
    }
};

function getFirstTwoPlayers(players_dict) {
    var min_ids = [];
    for (var key in players_dict) {
        // check if the property/key is defined in the object itself, not in parent
        console.log(key, players_dict[key]);
        min_ids.push(players_dict[key]);
    }
    var min_val1 = Math.min.apply(null, min_ids);
    min_ids.splice(min_ids.indexOf(min_val1), 1);
    var min_val2 = Math.min.apply(null, min_ids);

    var final_dict = {};

    for (var key in players_dict) {
        if (players_dict[key] === min_val1)
            final_dict[key] = 1;
        if (players_dict[key] == min_val2)
            final_dict[key] = 2;
    }
    return final_dict
};

var https = require("https"); // https server module
var url = require("url"); 
var fs = require("fs");  // file reading, writing, module
var path = require("path"); 
var ent = require("ent"); // blocks HTML characters 
    
// For https (key and certificate).
var hskey = fs.readFileSync("key.pem");
var hscert = fs.readFileSync("cert.pem");
var options = {
    key: hskey,
    cert: hscert,
    NPNProtocols: ["http/2.0", "spdy", "http/1.1", "http/1.0"],
};

var server_port = process.env.OPENSHIFT_NODEJS_PORT || 443
var server_ip_address = process.env.OPENSHIFT_NODEJS_IP || "192.168.1.2" //"212.251.17.119" 


var myApp = https.createServer(options, handleRequest);

myApp.listen(server_port, server_ip_address, function() { 
    console.log("Listening on " + server_ip_address + ", server_port " + server_port);
});

var io = require("socket.io")(myApp);
var player_names_ids = {};
var last_player_id = 0;
var game_started = false;

// All messages are handled here.
io.sockets.on("connection", function (socket, username) {
    socket.on("new_client", function (username) {
        socket.username = username;
        socket.broadcast.emit("new_client", username);

        last_player_id++;
        player_names_ids[username] = last_player_id;

        console.log("Player " + username + " joined");

        // If there are at least 2 players, assign them ID 1 and 2 and send message that game is ready.
        // Within this message it is sent also user name and user id of both players.
        if (!game_started && Object.keys(player_names_ids).length >= 2) {
            io.emit("game_ready_get_uids", getFirstTwoPlayers(player_names_ids));
            game_started = true;
        }
    });

    // Message in chat.
    socket.on("message", function (message) {
        message = ent.encode(message);
        socket.broadcast.emit("message", {username: socket.username, message: message});
    });

    // Reset game - either game was finished or someone pushed Reset button.
    socket.on("reset_game", function () {
        socket.broadcast.emit("reset_game");
        io.emit("game_ready_get_uids", getFirstTwoPlayers(player_names_ids));
        game_started = true;
    });

    // Move - User name, user ID and move position.
    socket.on("given_move", function (message) {
        var given_move = ent.encode(message.given_move);
        socket.broadcast.emit("given_move", {username: socket.username, user_id: message.user_id, given_move: given_move});
    });
    
    // Video data stream.
    socket.on("video call", function(data){
        /*console.log("video call type: ");
        console.log(data.type);
        console.log("candidate: ");
        console.log(data.candidate);
        console.log("description: ");
        console.log(data.description);*/
        socket.broadcast.emit("video call", data);
    });

    // If the player leaves, another player in array of waiting players can play.
    socket.on("disconnect", function() {
        if (socket.username) {
            console.log("Player " + socket.username + " left");

            // Find if the disconnected player was playing or not.
            // If yes, game will be considered as not started yet and it should.
            var curr_players = getFirstTwoPlayers(player_names_ids);
            if (socket.username in curr_players)
                game_started = false;
            else
                game_started = true;

            // Remove player from waiting queue containing player names and ids.
            // Then send a message about leaving to all - it will be outputted in common chat board.
            delete player_names_ids[socket.username];
            socket.broadcast.emit("client_left", socket.username);

            // There are still at least 2 players - they can play, procedure the same 
            // as when a new player comes. Only if the game didn't already started.
            // If one of current players left the game, it is restarted.
            if (!game_started && Object.keys(player_names_ids).length >= 2) {
                socket.broadcast.emit("reset_game");
                io.emit("game_ready_get_uids", getFirstTwoPlayers(player_names_ids));
                game_started = true;
            }
        }
    });
});
