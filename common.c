#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include "common.h"
int** init_board(int **board){
    board = malloc(sizeof(int*)* 3);
    for (int i = 0; i < 3; i++){
        board[i] = calloc(3, sizeof(int));
    }
    return board;
}
void print_board(int** board){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board[i][j] == 0){
                printf("%c ", '_');
            }
            else{
                printf("%c ", board[i][j] == 1 ? 'X' : 'O');
            }
        }
        printf("\n");
    }
}
int** msg_to_board(int** board, char buffer[]){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            board[i][j] = buffer[3*i + j] - '0';
        }
    }
    return board;
}
