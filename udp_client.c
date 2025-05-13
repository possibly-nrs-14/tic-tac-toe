#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.h"
#define BUF_SIZE 100
#define _ 0
#define X 1
#define O 2

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 0; 
    }
    int port_no = atoi(argv[1]);
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];
    char* draw = "Draw\n";
    char* start = "Both players joined. Starting game...\n";
    char* play_again = "Would you like to play again?\n";
    char* no_continue = "Your opponent did not wish to continue. Hence connection is being closed.\n";
    int** board;
    char* player1 = "Player 1, enter your move:\n";
    char* player2 = "Player 2, enter your move:\n";
    char* w1 = "Player 1 wins!\n";
    char* w2 = "Player 2 wins!\n";
    char* invalid = "Invalid move. Try again\n";

    board = init_board(board);
    int move[2] = {0};
    memset(buffer, 0, BUF_SIZE);
    memset(message, 0, BUF_SIZE);

    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Define the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = inet_addr(argv[2]);  // Server address (localhost)

    // Send initial message to server to register as a player
    sendto(sock, "Join", 4, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Waiting for game to start...\n");

    socklen_t addr_len = sizeof(server_addr);
    int cont = 1;

    while (cont) {
        int recv_len = recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (recv_len < 0) {
            perror("Receive failed");
            break;
        }

        if (strlen(buffer) == 9) {
            board = msg_to_board(board, buffer);
            print_board(board);
            printf("\n");
        } 
        else if (!strcmp(buffer, player1) || !strcmp(buffer, player2) || !strcmp(buffer, invalid)) {
            printf("%s", buffer);
            int row, col;
            scanf("%d %d", &row, &col);
            message[0] = row + '0';
            message[1] = col + '0';
            sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        } 
        else if (!strcmp(buffer, play_again)) {
            printf("%s", buffer);
            int choice;
            scanf("%d", &choice);
            message[0] = choice + '0';
            sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        } 
        else if (!strcmp(buffer, no_continue)) {
            printf("%s", buffer);
            cont = 0;
        } 
        else if (!strcmp(buffer, start) || !strcmp(buffer, draw) || !strcmp(buffer, w1) || !strcmp(buffer, w2)) {
            printf("%s", buffer);
        }

        memset(buffer, 0, BUF_SIZE);
        memset(message, 0, BUF_SIZE);
    }

    close(sock);
    for (int i = 0; i < 3; i++) {
        free(board[i]);
    }
    free(board);
}