#include "posix_web_console.h"
#include "posix_websocket.h"
#include <iostream>

// ^Purpose: Runs WebSocket server for console-like interaction with a single client

int main() {
    std::cout << "Attempt Access via http://localhost:8080/ \n";

    // Initialize WebSocket connection
    if (!init_websocket_connection()) {
        std::cerr << "Failed to initialize WebSocket connection\n";
        return 1;
    }

    // User-defined program
    printwc("Printing Message: Are you 6ft tall? y");
    
    std::string input;
    while (false) {//temp
        input = wcin();
        if (input == "y") {
            printwc("damn your kinda tall boi, like in a better than asian way.");        
            break;
        }
        else if (input == "n") {
            printwc("Well, romance isn't everything xD.");
            break;
        }
        else {
            printwc("invalid input, please try again with y or n as a response");
        }
    }
    
    printwc("Press any key to disconnect");
    wcin();

    printwc("You entered: " + input);
    printwc("Get disconnected bitch");

    // Close connection
close_websocket_connection();

    return 0;
}
