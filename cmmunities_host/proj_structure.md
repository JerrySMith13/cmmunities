
# /
- Used to represent the community's root directory
- Will contain all top-level config + subdirectories for data storage
- Config will have things like message TTL, allowed metadata, etc.
- All message data is stored in a single sqlite3 db file
- A "transaction" queue will be implemented to allow for insertion/deletion of messages

# /admin/
- Used to store data about user and message administration/censoring
- Data within is only accessible by users that are part of a certain group

# Path for a message

1. Message is typed and "sent" by user
2. Behind the scenes, TLS encodes the message + the user's key (a simple encoded username and password) and sends it to the server with all necessary information
3. The server recieves the message and checks it for validity (these set of checks are contained within their own subset of rules/functions)
4. If the message is valid, then the message gets added to the message transaction queue for the channel it's getting sent to.
5. The transaction thread adds the message to the respective database

# How do channels work?
- Channels are similar to a tree, or filesystem.
- Every community has a root directory for channels, and subdirectories may be added as needed.
- A community is similar to a git repo, or really any sort of unix-like filesystem
- Users/groups have different sets of permissions, and we could even use modes to set/get permissions
- Idea: message shell: login under a certain user and use commands like "ls" to find/navigate channels

# Where do I start?
- First of all, clean house. Project will start from the beginning
- Second, start with working on transactional structure for dbs. Adding channels, deleting them, posting messages, etc.
- Third, build from there. We can work on implementing users, the shell, maybe a TUI menu?
