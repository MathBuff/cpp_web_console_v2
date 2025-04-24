#include "console.h"
#include <iostream>

// ^Purpose: Runs WebSocket server for console-like interaction with a single client

int main() {
        std::cout << "Attempt Access via http://localhost:8080/ \n";
    sockaddr_in address;

    // Initialize WebSocket connection
    if (!init_websocket_connection(address)) {
        std::cerr << "Failed to initialize WebSocket connection\n";
        return 1;
    }

    // User-defined program
    printwc("Printing Messege: Are you 6ft tall? y/n");
    
    std::string input;
    while (true){
        input = wcin();
        if(input=="y"){
            printwc("damn your kinda tall boi, like in a better in asian way.");        
            break;
        }
        else if(input =="n"){
            printwc("Well, romance isn't everything xD.");
            break;
        }
        
        else{
            printwc("invalid input, please try again with y or n as a response");
        }
    }

    printwc("You entered: " + input);
    printwc("Get disconnected bitch");

    // Close connection
    close_websocket_connection();

    return 0;
}
