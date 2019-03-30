#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bludiste.h"

int solve(blud *maze) {
    buffer_t queue = {maze->size_x, maze->size_y, 0, -1, NULL, NULL, NULL}; // indexFu je -1, před 1. použitím se zvětší
    unsigned count; // kolik kroků

    queue.buff = malloc(sizeof(step) * maze->size_x * maze->size_y);
    queue.stackAc = malloc(sizeof(unsigned) * maze->size_x * maze->size_y);
    queue.stackFu = malloc(sizeof(unsigned) * maze->size_x * maze->size_y);

    if (queue.buff == NULL || queue.stackAc == NULL || queue.stackFu == NULL) {
        puts("Málo paměti");
        exit(1);
    }
    initBuff(&queue);
    queue.stackAc[0] = 0;

    while (queue.buff[(queue.size_x * queue.size_y) - 1].depth == -1) {
        makeSteps(&queue, maze);
    }
    count = findRoute(&queue, maze);
    //printBlud(maze);
    //printf("%d\n", count);

    free(queue.buff);
    free(queue.stackAc);
    free(queue.stackFu);

    return count;
}

void initBuff(buffer_t *buff) {
    int i, j;

    for (i = 0; i < buff->size_y; i++) {
        for (j = 0; j < buff->size_x; j++) {
            buff->buff[i * buff->size_x + j].depth = -1;
            buff->buff[i * buff->size_x + j].parent = -1;
        }
    }
    buff->buff[0].depth = 0;
    buff->buff[0].parent = 0;
}

void makeSteps(buffer_t *queue, blud *maze) {
    unsigned int i, *swapS;

    for (i = 0; i <= queue->indexAc; i++) {
        solveStep(queue, maze, i);
    }
    swapS = queue->stackAc;
    queue->stackAc = queue->stackFu;
    queue->stackFu = swapS;
    queue->indexAc = queue->indexFu;
    queue->indexFu = -1;
}

void solveStep(buffer_t *queue, blud *maze, int index) {
    int steps[4] = {0, 0, 0, 0};
    int i;

    steps[0] = queue->stackAc[index] - queue->size_x;
    steps[1] = queue->stackAc[index] + 1;
    steps[2] = queue->stackAc[index] + queue->size_x;
    steps[3] = queue->stackAc[index] - 1;
    for (i = 0; i < 4; i++) { // Vyhození nepotřebných dat
        if (steps[i] < 0 || steps[i] >= queue->size_x * queue->size_y){ //Kontrola mezí pole horní a dolní
            DISCARD;
        }
        if ((steps[i] + 1) % queue->size_x == 0 && i == 3) { // Kontrola bočních stěn přechod zdola
            DISCARD;
        }
        if (steps[i] % queue->size_x == 0 && i == 1) { //Kontrola bočních stěn přechod zhora
            DISCARD;
        }
        if (maze->bludiste[steps[i]] == WALL) { // Kontrola zdí
            DISCARD;
        }
        if ((queue->buff[queue->stackAc[index]].depth + 1) >= queue->buff[steps[i]].depth && queue->buff[steps[i]].depth != -1) { // Už jsem tam byl
            DISCARD;
        }
    }
    for (i = 0; i < 4; i++) {
        if (steps[i] == -1) {
            continue;
        }
        queue->indexFu++; // zvyš index příštích bodů o 1
        queue->stackFu[queue->indexFu] = steps[i]; // ulož tam aktuální bod
        queue->buff[steps[i]].depth = queue->buff[queue->stackAc[index]].depth + 1; // ulož do něj hloubku
        queue->buff[steps[i]].parent = queue->stackAc[index]; // a rodiče
    }
}

unsigned findRoute(buffer_t *queue, blud *maze) {
    int position = queue->size_x * queue->size_y - 1;

    //printf("%d\n", queue->buff[position].depth);
    while (position) {
        //maze->bludiste[position] = ROUTE;
        position = queue->buff[position].parent;
    }
    //maze->bludiste[position] = ROUTE;

    return queue->buff[queue->size_x * queue->size_y - 1].depth + 1;
}

blud storeBlud(void) {
    int i;
    blud maze = {0, 0, NULL};

    scanf("%d %d\n", &maze.size_y, &maze.size_x);
    maze.bludiste = malloc(sizeof(char) * maze.size_x * maze.size_y);

    for (i = 0; i < maze.size_y * maze.size_x; i++) {
        scanf(" %c", &maze.bludiste[i]);
    }

    return maze;
}

void printBlud(blud *maze) {
    int i, j;

    for (i = 0; i < maze->size_y; i++) {
        for (j = 0; j < maze->size_x; j++) {
            putchar(*(maze->bludiste + (i * maze->size_x + j)));
        }
        putchar('\n');
    }
}

void makeMaze(unsigned size_x, unsigned size_y, unsigned lenght) {
    unsigned i, ALenght;
    unsigned last = 0;
    int x, y;
    blud maze = {size_x, size_y, NULL};

    if (lenght < size_x + size_y - 1) {
        puts("Nejde to.");
        return;
    }
    maze.bludiste = malloc(sizeof(char) * size_x * size_y);
    if (maze.bludiste == NULL) {
        puts("Málo paměti");
        exit(1);
    }
    for (i = 0; i < maze.size_x * maze.size_y; i++) {
        maze.bludiste[i] = CORIDOR;
    }
    if (maze.size_y >= 5 && maze.size_x != 1) {
        for (i = 0; i < (maze.size_x - 1); i++) {
            maze.bludiste[i + maze.size_x] = WALL;
        }
        last = maze.size_x + maze.size_x - 2;
    }
    else if (maze.size_y > 1 && maze.size_x != 1) {
        for (i = 0; i < (maze.size_y - 1); i++) {
            maze.bludiste[1 + i * maze.size_x] = WALL;
        }
        last = 1 + (maze.size_y - 2) * maze.size_x;
    }
    if (maze.size_y == 1 && lenght != maze.size_x) {
        puts("Nejde to.");
        return;
    }
    if (maze.size_x == 1 && lenght != maze.size_y) {
        puts("Nejde to.");
        return;
    }
    while (1) {
        ALenght = solve(&maze);
        if (ALenght < lenght) {
            if (maze.size_y >= 5?addWall(&maze, &last):addWallVertical(&maze, &last) == -1) {
                puts("Nejde to.");
                break;
            }
            continue;
        }
        else if (ALenght == lenght) {
            x = last % maze.size_x;
            y = last / maze.size_x;
            if (maze.size_y > 1 && maze.size_x > 1) {
                if (maze.size_y >= 5) {
                    if (maze.bludiste[y * maze.size_x] == WALL) {
                        if (x < maze.size_x - 2) {
                            makeWall(&maze, last - 2 * maze.size_x + 2, 1);
                        }
                    }
                    else {
                        if (x > 1) {
                            makeWall(&maze, last - 2 * maze.size_x - 2, 1);
                        }
                    }
                }
                else if (maze.size_y > 1){
                    if (maze.bludiste[x] == WALL) {
                        if (y < maze.size_y - 2) {
                            makeWallVertical(&maze, last - 2 + 2 * maze.size_x, 1);
                        }
                    }
                    else {
                        if (y > 1) {
                            makeWallVertical(&maze, last - 2 * maze.size_x - 2, 1);
                        }
                    }
                }
            }
            if (maze.size_y >= 5 && maze.size_x > 1) {
                if (lenght == maze.size_x + maze.size_y - 1) {
                    makeWall(&maze, last, 1);
                }
                else {
                    last = makeWall(&maze, last, 0);
                    makeWallVertical(&maze, last, 1);
                }
            }
            else if (maze.size_y > 1 && maze.size_x > 1) {
                if (lenght == maze.size_x + maze.size_y - 1) {
                    makeWallVertical(&maze, last, 1);
                }
                else {
                    last = makeWallVertical(&maze, last, 0);
                    makeWall(&maze, last, 1);
                }
            }
            printBlud(&maze);
            break;
        }
        else if (ALenght > lenght) {
            puts("Nejde to.");
            break;
        }
    }
    free(maze.bludiste);
}

int addWall(blud *maze, unsigned *last) {
    unsigned x, y;

    x = *last % maze->size_x;
    y = *last / maze->size_x;
    if (maze->bludiste[maze->size_x * y] == WALL) { // Když je na začátku zeď
        if (x == maze->size_x - 2) {
            y += 2;
            x = maze->size_x - 1;
            *last = x + maze->size_x * y;
            if (y >= maze->size_y - 1) {
                return -1;
            }
            else {
                maze->bludiste[*last] = WALL;
                return 0;
            }
        }
        else {
            (*last)++;
            maze->bludiste[*last] = WALL;
            return 0;
        }
    }
    else { // Když je na konci
        if (x == 1) {
            y += 2;
            x = 0;
            *last = x + maze->size_x * y;
            if (y >= maze->size_y - 1) {
                return -1;
            }
            else {
                maze->bludiste[*last] = WALL;
                return 0;
            }
        }
        else {
            (*last)--;
            maze->bludiste[*last] = WALL;
            return 0;
        }
    }
}

int addWallVertical(blud *maze, unsigned *last) {
    unsigned x, y;

    x = *last % maze->size_x;
    y = *last / maze->size_x;
    if (maze->bludiste[x] == WALL) { // Když je na hoře zeď
        if (y == maze->size_y - 2) {
            x += 2;
            y = maze->size_y - 1;
            *last = x + maze->size_x * y;
            if (x >= maze->size_x - 1) {
                return -1;
            }
            else {
                maze->bludiste[*last] = WALL;
                return 0;
            }
        }
        else {
            *last += maze->size_x;
            maze->bludiste[*last] = WALL;
            return 0;
        }
    }
    else { // Když je dole
        if (y == 1) {
            x += 2;
            y = 0;
            *last = x + maze->size_x * y;
            if (x >= maze->size_x - 1) {
                return -1;
            }
            else {
                maze->bludiste[*last] = WALL;
                return 0;
            }
        }
        else {
            *last -= maze->size_x;
            maze->bludiste[*last] = WALL;
            return 0;
        }
    }
}

unsigned makeWall(blud *maze, unsigned last, int end) {
    for (; last / maze->size_x < maze->size_y - 1; last += maze->size_x) {
        maze->bludiste[last] = WALL;
    }
    if (end) {
        maze->bludiste[last] = WALL;
    }
    else {
        last -= maze->size_x;
    }
    return last;
}

unsigned makeWallVertical(blud *maze, unsigned last, int end) {
    int check = last;
    for (; (last % maze->size_x) < (maze->size_x - 1); last++) {
        maze->bludiste[last] = WALL;
    }
    if (end) {
        maze->bludiste[last] = WALL;
    }
    else if (last != check){
        last--;
    }
    return last;
}

int main() {
    int S, W, H, P, C;
    int X, Y, x, y;
    int i, j;

    scanf("%d%d%d%d%d", &S, &W, &H, &P, &C);
    blud mesto = {W, H, NULL};
    mesto.bludiste = calloc(sizeof(int), W*H);
    int route[C][4];
    for (; P > 0; P--) {
        scanf("%d%d", &X, &Y);
        mesto.bludiste[X + Y*W] = WALL;
    }
    for (i = 0; i < C; i++) {
        for (j = 0; j < 4; j++) {
            scanf("%d", &route[i][j]);
        }
    }
    return 0;
}
