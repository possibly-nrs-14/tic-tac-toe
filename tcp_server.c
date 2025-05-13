#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include "common.h"
#define BUF_SIZE 100
#define MAX_CLIENTS 2  // Maximum number of clients
#define _ 0 
#define X 1
#define O 2 
int winner = 0;
int play_mode = 1;

int horizontal_check(int** board, int row, int piece){
    for(int i = 0; i < 3; i++){
        if (board[row][i] != piece){
            return 0;
        }
    }
    return 1;
}
int vertical_check(int** board, int col, int piece){
    for(int i = 0; i < 3; i++){
        if (board[i][col] != piece){
            return 0;
        }
    }
    return 1;
}
int diagonal_check(int** board, int diag, int piece){
    if (!diag){
        for(int i = 0; i < 3; i++){
            if (board[i][i] != piece){
                return 0;
            }
        }
        return 1;
    } 
    else{
        for(int i = 0; i < 3; i++){
            if (board[i][2 - i] != piece){
                return 0;
            }
        }
        return 1;
    } 

}
int win_check(int** board){
    int pieces[2];
    pieces[0] = X;
    pieces[1] = O;
    for(int i = 0; i < 2; i++){
        for(int j = 0;  j < 3; j++){
            if (horizontal_check(board, j, pieces[i]) || vertical_check(board, j, pieces[i])){
                winner = pieces[i];
                return 1;
            }
        }
        if (diagonal_check(board, i, pieces[i]) || diagonal_check(board, !i, pieces[i])){
            winner = pieces[i];
            return 1;
        }
    }
    return 0;
}
int draw_check(int** board){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if (board[i][j] == _){
                return 0;
            }
        }   
    }
    return 1;
}


int main(int argc, char* argv[]){
    if (argc != 2) {
        return 0; 
    }
    c
    char* start = "Both players joined. Starting game...\n";
    char* player1 = "Player 1, enter your move:\n";
    char* play_again = "Would you like to play again?\n";
    char* no_continue = "Your opponent did not wish to continue. Hence connection is being closed.\n";
    char* draw = "Draw\n";
    char* invalid = "Invalid move. Try again\n";
    int server_fd, client_sock, max_sd, activity, new_socket, valread;
    int client_sockets[MAX_CLIENTS];  // Array to keep track of client sockets
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];
    int** board;
    board = init_board(board);

    // fd_set structures for select()
    fd_set readfds;

    // Initialize all client sockets to 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }
    for(int i = 0; i < BUF_SIZE; i++){
        buffer[i] = 0;
        message[i] = 0;
    }

    // Create a server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_no);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind done.\n");

    // Listen for incoming connections
    if (listen(server_fd, 2) < 0) {
        perror("Listen failed");
        close(server_fd);
    }
    printf("Waiting for incoming connections...\n");

    addr_len = sizeof(client_addr);
    int cont = 1;
    int choices[2];
    choices[0] = -1;
    choices[1] = -1;
    int curr_player = 0;
    int a = 0;
    while (cont) {
        // Clear the read set and add the server socket
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets to the set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
            }
            // Find the maximum socket descriptor number for select()
            if (client_sockets[i] > max_sd) {
                max_sd = client_sockets[i];
            }
        }

        // Wait for activity on any socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("Select error");
        }

        // If something happened on the server socket, it's an incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (new_socket < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }
            printf("New connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            client_sockets[a] = new_socket;
            printf("Added to client_sockets at index %d\n", a);
            a++;
        }
        if(a == 2){
            a = 0;
            strcpy(message, start);
            send(client_sockets[0], message, strlen(message), 0);  
            send(client_sockets[1], message, strlen(message), 0);  
            for(int j = 0; j < BUF_SIZE; j++){
                message[j] = 0;
            }
            sleep(2);
            for(int j = 0; j < 3; j++){
                for(int k = 0; k < 3; k++){
                    char c = board[j][k] + '0';
                    message[3*j + k] = c;
                }
            }
            send(client_sockets[0], message, strlen(message), 0);  
            send(client_sockets[1], message, strlen(message), 0); 
            for(int j = 0; j < BUF_SIZE; j++){
                message[j] = 0;
            }
            sleep(2);
            strcpy(message, player1);
            send(client_sockets[0], message, strlen(message), 0);
            curr_player = !curr_player;
            for(int j = 0; j < BUF_SIZE; j++){
                message[j] = 0;
            }
        }

        // Handle I/O for each client socket
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {
                // Read from the client
                int num_bytes = read(client_sockets[i], buffer, BUF_SIZE);
                if (num_bytes == 0) {
                    // Client disconnected, close the socket and remove it
                    printf("Client %d disconnected.\n", i);
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                } 
                else{
                    if (play_mode){
                        int row = buffer[0] - '0';
                        int col = buffer[1] - '0';
                        int recvd_from = !curr_player;
                        if (board[row - 1][col - 1] == _  && 1<= row <= 3 && 1<= col <=3){
                            board[row - 1][col - 1] = recvd_from == 0 ? X : O;
                            if (win_check(board)){
                                play_mode = 0;
                                for(int j = 0; j < 3; j++){
                                    for(int k = 0; k < 3; k++){
                                        char c = board[j][k] + '0';
                                        message[3*j + k] = c;
                                    }
                                }
                                send(client_sockets[0], message, strlen(message), 0);
                                send(client_sockets[1], message, strlen(message), 0); 
                                for(int j = 0; j < BUF_SIZE; j++){
                                    message[j] = 0;
                                } 
                                snprintf(message, sizeof(message), "Player %d wins!\n", winner);
                                sleep(2);
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0);  
                                for(int j = 0; j < BUF_SIZE; j++){
                                    message[j] = 0;
                                } 
                                strcpy(message, play_again);
                                sleep(2);
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0);

                            }
                            else if(draw_check(board)){
                                play_mode = 0;
                                for(int j = 0; j < 3; j++){
                                    for(int k = 0; k < 3; k++){
                                        char c = board[j][k] + '0';
                                        message[3*j + k] = c;
                                    }
                                }
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0); 
                                for(int j = 0; j < BUF_SIZE; j++){
                                    message[j] = 0;
                                } 
                                sleep(2);
                                strcpy(message, draw);
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0);  
                                for(int j = 0; j < BUF_SIZE; j++){
                                    message[j] = 0;
                                } 
                                sleep(2);
                                strcpy(message, play_again);
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0); 
                            }
                            else{
                                for(int j = 0; j < 3; j++){
                                    for(int k = 0; k < 3; k++){
                                        char c = board[j][k] + '0';
                                        message[3*j + k] = c;
                                    }
                                }
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0); 
                                for(int j = 0; j < BUF_SIZE; j++){
                                    message[j] = 0;
                                }
                                sleep(2);
                                // printf("curr_player is %d\n", curr_player);
                                snprintf(message, sizeof(message), "Player %d, enter your move:\n", curr_player + 1);
                                send(client_sockets[curr_player], message, strlen(message), 0);
                                curr_player = !curr_player;                                
                            }
                        }
                        else{
                            strcpy(message, invalid);
                            send(client_sockets[recvd_from], message, strlen(message), 0);  
                        }
                    }
                    else{
                        int choice = buffer[0] - '0';
                        choices[i] = choice;
                        if (choices[0] != -1 && choices[1] != -1){
                            if (choices[0] && choices[1]){
                                curr_player = 0;  
                                choices[0] = -1;
                                choices[1] = -1;
                                printf("Resetting board for new game...\n"); 
                                for(int j = 0; j < 3; j++){
                                    for(int k = 0; k < 3; k++){
                                        board[j][k] = 0;
                                        char c = board[j][k] + '0';
                                        message[3*j + k] = c;
                                    }
                                }
                                send(client_sockets[0], message, strlen(message), 0);  
                                send(client_sockets[1], message, strlen(message), 0);
                                for(int j = 0; j < BUF_SIZE; j++){
                                    message[j] = 0;
                                }
                                sleep(2);
                                strcpy(message, player1);
                                send(client_sockets[0], message, strlen(message), 0);
                                curr_player = !curr_player;
                            }
                            else if (!choices[0] && !choices[1]){
                                snprintf(message, strlen(message), "Closing connection...\n"); 
                                close(client_sockets[0]);
                                close(client_sockets[1]);
                                cont = 0;
                            }
                            else{
                                strcpy(message, no_continue);
                                if (choices[0]){
                                    send(client_sockets[0], message, strlen(message), 0);
                                }
                                else if(choices[1]){
                                    send(client_sockets[1], message, strlen(message), 0);
                                }
                                close(client_sockets[0]);
                                close(client_sockets[1]);
                                cont = 0;
                            }
                            play_mode = 1;
                        }
                            
                    }
                    
                }
            }
        }
        for(int i = 0; i < BUF_SIZE; i++){
            buffer[i] = 0;
            message[i] = 0;
        }
    }
    close(server_fd);
    for(int i = 0; i < 3; i++){
        free(board[i]);
    }
    free(board);
}
