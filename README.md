# Tic-tac-toe game implementation in C using sockets
This game has beem implemented separately with both TCP and UDP protocols.
## File Structure  & Instructions
The following files and directories can be found:
1. **`tcp_client.c`**: A TCP client implementation for the tic-tac-toe board game.
2. **`tcp_server.c`**: A TCP server implementation for the tic-tac-toe board game.
3. **`udp_client.c`**: A UDP client implementation for the tic-tac-toe board game.
4. **`udp_server.c`**: A UDP server implementation for the tic-tac-toe board game.
5. **`common.c`**: Functions used in all of the above files.
6. **`common.h`**: Header file of `common.c`


For the tic-tac-toe game, you must have 3 terminals ready: one for each client & one for the server.

To run the game, run this for TCP:
```
gcc tcp_client.c -o client
gcc tcp_server.c -o server
./server <port_no> # run this once in one terminal
./client <port_no> <server_ip_address> # run this twice, once each in the other 2 terminals
```

To run the game, run this for UDP:
```
gcc udp_client.c -o client
gcc udp_server.c -o server
./server <port_no> # run this once in one terminal
./client <port_no> <server_ip_address> # run this twice, once each in the other 2 terminals
```

For part B, you need 2 terminals:

```
gcc client.c -o client
gcc server.c -o server
./server  # run this once in one terminal
./client # run this once in the other terminal
```

Note: Use the same port no. for client and server.
