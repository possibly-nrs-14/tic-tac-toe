#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
int** init_board(int **board);
void print_board(int** board);
int** msg_to_board(int** board, char buffer[]);