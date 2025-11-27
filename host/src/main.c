//  Daemon:
    //      Input: This is boilerplate daemon startup. read a toml file (conf or WHATEVER) and parse it to object
    //      Use object to determine which port to listen on, which server identifiers are valid and their matching CONSTANT SERVER IDS     
    //      Open database files and load caches into memory
    //      Output: Begin event loop
    //         match requests to their "Domains" and start that mini-service
    //         Use special caching algorithm and keep protocol messages SMALL for low latency, real time messaging
    //         when a message is recieved, main daemon locates which cmmunity it belongs to and runs "internal host", forwarding all message parameters into it

    //  Internal host:
    //      Input: structured data from requestd (think of this as your server's endpoint, process data and return it w/ same structure)
    //      Route: determine where the request's parameters should go
    //      Change: change databases, message logs, etc.
    //      Output: Output request status and requested data
    //      NOTE: MAYBE FORK FROM DAEMON TO HERE, MAINTAIN REQUEST DATA W/O IPC??

    // Internal Editor (for "repos", like git command)
    //      Input: parse commands in userland, "edit" both remote and local repos
    //      Change: Change data about either remote or local repo, i.e. creation, 
    //      Output: Display change in real time, show community data (This is detailed in the Client subsection)

    //  Client (for real time communications, reading pager posts, etc.)
    //      input: real-time UI menus
    //      change: change repo in real time, like a shell
    //      output: write to UI


int main(int nargs, char** args){
    



}




//  cm command
//  step 1: parse verb
//      possible verbs may include init, send, read, mkdir, touch, ls
//  step 2: make change: change db files, update settings w/ daemon interactions, broadcast, etc.
//  step 3: return status: give data about certain statuses of channels, etc.


// Verb parsing: 
// step 1: for non-static requests, use cwd for cmmunity root
// step 2: use cwd + data recieved in invocation PLUS global config file to create request context
// step 3: update data in repo and return requested data


/*

---- FRAMEWORKING: the usability ----
Data: client-side - conf folder (similar to manifest) that tells client where to talk to people, what kind of conf the server uses, etc.
Data: server-side - folder similar to .git which holds message data


*/