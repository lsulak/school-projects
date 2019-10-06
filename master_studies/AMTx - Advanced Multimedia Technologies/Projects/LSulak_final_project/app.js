/*
 * Author:      Ladislav Sulak (laco.sulak@gmail.com)
 * Subject:     Final project in the subject Advanced Multimedia Technologies - TEI Crete.
 * Description: Web application using node.js and socket.io - encrypted chat, video, microphone
 *              and Google map with coordinates.
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
var server_ip_address = process.env.OPENSHIFT_NODEJS_IP || "192.168.1.4" // "212.251.17.119"

var myApp = https.createServer(options, handleRequest);

myApp.listen(server_port, server_ip_address, function() { 
    console.log("Listening on " + server_ip_address + ", server_port " + server_port);
});

var user_info_storage = {};

var io = require("socket.io")(myApp);

// All messages are handled here.
io.sockets.on("connection", function (socket, username) {

    socket.on("new_client", function (username) {
        console.log("User '" + username + "' joined");
        socket.username = username;

        user_info_storage[username] = {"last_picture": "", "last_location": ""};
        socket.broadcast.emit("new_client", username);

        var op_name = "";
        for (var name in user_info_storage) {
            if (name == socket.username)
                continue;

            else
                op_name = name;
        }
        if (op_name.length > 0) {
            var other_client_screenshot = user_info_storage[op_name]["last_picture"];
            var other_client_location = user_info_storage[op_name]["last_location"];

            socket.emit("old_user_data", 
                {
                    username: op_name, 
                    location: other_client_location, 
                    profile_picture: other_client_screenshot
                }
            );
        }
    });

    // Message in chat.
    socket.on("message", function (message) {
        message = ent.encode(message);
        socket.broadcast.emit("message", {username: socket.username, message: message});
    });

    socket.on("in_radius", function (is_in_radius) {
        socket.broadcast.emit("in_radius", is_in_radius);
    })

    // Map - User name and coordinates.
    socket.on("position_in_map", function (message) {
        user_info_storage[socket.username]["last_location"] = message;
        socket.broadcast.emit("position_in_map", message);
    });
    
     // Profile picture - User name and image data.
    socket.on("new_profile_picture", function (data) {
        console.log("Received screen from " + data.username);

        user_info_storage[socket.username]["last_picture"] = data.new_profile_picture;
        socket.broadcast.emit("new_profile_picture", data);
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

    // If the client leaves, all other clients will be notified.
    socket.on("disconnect", function() {
        if (socket.username) {
            console.log("User '" + socket.username + "' left");

            delete user_info_storage[socket.username];
            socket.broadcast.emit("client_left", socket.username);
        }
    });
});
