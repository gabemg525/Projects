Multi-Threaded HTTP Server
Gabriel Gorospe

httpserver.c

source file for an http server with concurrency handling using threading. Uses a hash table to store reader writer locks for when multiple requests want to access a single file

queue.h

header file for queue data structure, includes push, pop, and initializing functions

rwlock.h

header file for an read/write lock with that can be used on concurrent operations

connect.h

header file implementing functions which handle socket connections

debug.h

header file implementing debugging

response.h

header file implementing functions for sending responses to client

