#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#include "common.h"

#define BUF_SIZE 100
#define MAX_CLIENTS 2
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
    int port_no = atoi(argv[1]);
    char* start = "Both players joined. Starting game...\n";
    char* player1 = "Player 1, enter your move:\n";
    char* play_again = "Would you like to play again?\n";
    char* no_continue = "Your opponent did not wish to continue. Hence connection is being closed.\n";
    char* draw = "Draw\n";
    char* invalid = "Invalid move. Try again\n";
    
    int server_fd;
    struct sockaddr_in server_addr, client_addr[MAX_CLIENTS];
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];
    int** board;
    board = init_board(board);

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_no);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind done.\n");

    printf("Waiting for players...\n");

    int client_count = 0;
    int curr_player = 0;
    int choices[2] = {-1, -1};
    int cont = 1;

    while (cont) {
        if (client_count < MAX_CLIENTS) {
            recvfrom(server_fd, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr[client_count], &addr_len);
            printf("New player connected.\n");
            client_count++;

            if (client_count == MAX_CLIENTS) {
                // Start the game
                sendto(server_fd, start, strlen(start), 0, (struct sockaddr *)&client_addr[0], addr_len);
                sendto(server_fd, start, strlen(start), 0, (struct sockaddr *)&client_addr[1], addr_len);
                for(int j = 0; j < 3; j++) {
                    for(int k = 0; k < 3; k++) {
                        message[3*j + k] = board[j][k] + '0';
                    }
                }
                sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[0], addr_len);
                sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[1], addr_len);
                
                // Prompt first player
                sendto(server_fd, player1, strlen(player1), 0, (struct sockaddr *)&client_addr[0], addr_len);
            }
        } 
        else {
            recvfrom(server_fd, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr[curr_player], &addr_len);
            
            if (play_mode) {
                int row = buffer[0] - '0';
                int col = buffer[1] - '0';
                
                if (board[row - 1][col - 1] == _ && 1 <= row <= 3 && 1 <= col <= 3) {
                    board[row - 1][col - 1] = curr_player == 0 ? X : O;
                    for(int j = 0; j < 3; j++) {
                        for(int k = 0; k < 3; k++) {
                            message[3*j + k] = board[j][k] + '0';
                        }
                    }
                    sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[0], addr_len);
                    sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[1], addr_len);

                    if (win_check(board)) {
                        play_mode = 0;
                        sprintf(message, "Player %d wins!\n", winner);
                        sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[0], addr_len);
                        sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[1], addr_len);
                        sendto(server_fd, play_again, strlen(play_again), 0, (struct sockaddr *)&client_addr[0], addr_len);
                        sendto(server_fd, play_again, strlen(play_again), 0, (struct sockaddr *)&client_addr[1], addr_len);
                    } 
                    else if (draw_check(board)) {
                        play_mode = 0;
                        sendto(server_fd, draw, strlen(draw), 0, (struct sockaddr *)&client_addr[0], addr_len);
                        sendto(server_fd, draw, strlen(draw), 0, (struct sockaddr *)&client_addr[1], addr_len);
                        sendto(server_fd, play_again, strlen(play_again), 0, (struct sockaddr *)&client_addr[0], addr_len);
                        sendto(server_fd, play_again, strlen(play_again), 0, (struct sockaddr *)&client_addr[1], addr_len);
                    } 
                    else {
                        curr_player = !curr_player;
                        sprintf(message, "Player %d, enter your move:\n", curr_player + 1);
                        sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[curr_player], addr_len);
                    }
                } 
                else {
                    sendto(server_fd, invalid, strlen(invalid), 0, (struct sockaddr *)&client_addr[curr_player], addr_len);
                }
            } 
            else {
                int choice = buffer[0] - '0';
                choices[curr_player] = choice;
                curr_player = !curr_player;
                if (choices[0] != -1 && choices[1] != -1) {
                    if (choices[0] && choices[1]) {
                        curr_player = 0;
                        choices[0] = -1;
                        choices[1] = -1;
                        play_mode = 1;
                        for(int j = 0; j < 3; j++) {
                            for(int k = 0; k < 3; k++) {
                                board[j][k] = 0;
                                message[3*j + k] = '0';
                            }
                        }
                        sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[0], addr_len);
                        sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr[1], addr_len);
                        sleep(2);
                        sendto(server_fd, player1, strlen(player1), 0, (struct sockaddr *)&client_addr[0], addr_len);
                    } 
                    else {
                        int wanted_to_play = choices[0] == 1 ? 0 : 1;
                        sendto(server_fd, no_continue, strlen(no_continue), 0, (struct sockaddr *)&client_addr[wanted_to_play], addr_len);
                        cont = 0;
                    }
                }
            }
        }

        for(int i = 0; i < BUF_SIZE; i++){
            message[i] = 0;
            buffer[i] = 0;
        }
    }

    close(server_fd);
    for(int i = 0; i < 3; i++) {
        free(board[i]);
    }
    free(board);
}