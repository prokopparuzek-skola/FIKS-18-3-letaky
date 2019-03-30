#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bludiste.h"

void solve(blud *maze, int route[4]) {
    if (route[0] == route[2] && route[1] == route[3]) { // Pokud je cíl stejný jako start, přeskoč
        maze->bludiste[route[1]*maze->size_x + route[0]]++;
        return;
    }
    buffer_t queue = {maze->size_x, maze->size_y, 0, -1, NULL, NULL, NULL, NULL}; // indexFu je -1, před 1. použitím se zvětší
    int back = 1; // návratová hodnota solveStep

    queue.buff = malloc(sizeof(step) * maze->size_x * maze->size_y);
    queue.stackAc = malloc(sizeof(unsigned) * maze->size_x * maze->size_y);
    queue.stackFu = malloc(sizeof(unsigned) * maze->size_x * maze->size_y);
    queue.route = route;

    if (queue.buff == NULL || queue.stackAc == NULL || queue.stackFu == NULL) {
        puts("Málo paměti");
        exit(1);
    }
    initBuff(&queue, route);
    queue.stackAc[0] = route[1]*queue.size_x + route[0];

    while (back == 1) {
        back = makeSteps(&queue, maze, route);
    }
    if (back == 2) {
        free(queue.buff);
        free(queue.stackAc);
        free(queue.stackFu);
        return;
    }
    findRoute(&queue, maze, route);

    free(queue.buff);
    free(queue.stackAc);
    free(queue.stackFu);
}

void initBuff(buffer_t *buff, int *route) {
    int i, j;

    for (i = 0; i < buff->size_y; i++) {
        for (j = 0; j < buff->size_x; j++) {
            buff->buff[i * buff->size_x + j].depth = -1;
            buff->buff[i * buff->size_x + j].parent = -1;
        }
    }
    buff->buff[route[1]*buff->size_x + route[0]].depth = 0;
    buff->buff[route[1]*buff->size_x + route[0]].parent = 0;
}

int makeSteps(buffer_t *queue, blud *maze, int *route) {
    unsigned int i, *swapS, test = 0;
    int back; // návratová hodnota solveStep

    for (i = 0; i <= queue->indexAc; i++) {
        back = solveStep(queue,maze,  i, route); // hledá všechny možné cesty pro 1 vrchol grafu
        if (back == 0) { // jsi u cíle, dej vědět
            return 0;
        }
        if (back != 2) {
            test = 1;
        }
    }
    if (test == 1) { // když se nějaké cesty našly prohodí body, takže příště se bude hledat to co se našlo
        swapS = queue->stackAc;
        queue->stackAc = queue->stackFu;
        queue->stackFu = swapS;
        queue->indexAc = queue->indexFu;
        queue->indexFu = -1;
        return 1;
    }
    return 2; // když ne, dá vědět
}

void city1(buffer_t *queue, int index, int steps[4]){
    steps[0] = queue->stackAc[index] - queue->size_x;   // nahoru
    steps[3] = queue->stackAc[index] + 1;               // doprava
    steps[1] = queue->stackAc[index] + queue->size_x;   // dolu
    steps[2] = queue->stackAc[index] - 1;               // doleva
}

void city2(buffer_t *queue, int index, int steps[4]){
    steps[3] = queue->stackAc[index] - queue->size_x;   // nahoru
    steps[0] = queue->stackAc[index] + 1;               // doprava
    steps[2] = queue->stackAc[index] + queue->size_x;   // dolu
    steps[1] = queue->stackAc[index] - 1;               // doleva
}

void city3(buffer_t *queue, int index, int steps[4]){
    steps[1] = queue->stackAc[index] - queue->size_x;   // nahoru
    steps[3] = queue->stackAc[index] + 1;               // doprava
    steps[2] = queue->stackAc[index] + queue->size_x;   // dolu
    steps[0] = queue->stackAc[index] - 1;               // doleva
}

void city4(buffer_t *queue, int index, int steps[4]){
    steps[2] = queue->stackAc[index] - queue->size_x;   // nahoru
    steps[1] = queue->stackAc[index] + 1;               // doprava
    steps[0] = queue->stackAc[index] + queue->size_x;   // dolu
    steps[3] = queue->stackAc[index] - 1;               // doleva
}

int solveStep(buffer_t *queue, blud *maze, int index, int *route) {
    int steps[4] = {0, 0, 0, 0};
    int i, test = 0;

    fav(queue, index, steps);

    for (i = 0; i < 4; i++) { // Vyhození nepotřebných dat
        if (steps[i] < 0 || steps[i] >= queue->size_x * queue->size_y){ //Kontrola mezí pole horní a dolní
            DISCARD;
        }
        if (queue->stackAc[index] % queue->size_x == 0 && steps[i] % queue->size_x == queue->size_x - 1) { // Kontrola bočních stěn přechod zdola
            DISCARD;
        }
        if (queue->stackAc[index] % queue->size_x == queue->size_x - 1 && steps[i] % queue->size_x == 0) { //Kontrola bočních stěn přechod zhora
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
        test = 1;
        queue->indexFu++; // zvyš index příštích bodů o 1
        queue->stackFu[queue->indexFu] = steps[i]; // ulož tam aktuální bod
        queue->buff[steps[i]].depth = queue->buff[queue->stackAc[index]].depth + 1; // ulož do něj hloubku
        queue->buff[steps[i]].parent = queue->stackAc[index]; // a rodiče
        if (steps[i] == route[3]*queue->size_x + route[2]) {
            return 0; // jsi u cíle, dej vědět
        }
    }
    return test?1:2; // ne dej vědět zda jsi něco našel(1), nebo ne(2)
}

void findRoute(buffer_t *queue, blud *maze, int* route) {
    int position = route[3]*queue->size_x + route[2];

    while (position != route[1]*queue->size_x + route[0]) {
        maze->bludiste[position]++;
        position = queue->buff[position].parent;
    }
    maze->bludiste[position]++;
}

void color(blud *maze) {
    int i, max = 0;

    for (i = 0; i < maze->size_x * maze->size_y; i++) { // spočte maximální nasycenost
        if (maze->bludiste[i] > max) {
            max = maze->bludiste[i];
        }
    }
    for (i = 0; i < maze->size_x * maze->size_y; i++) {
        if (maze->bludiste[i] != WALL) {
            maze->bludiste[i] = ceil(255.0 * maze->bludiste[i] / max);
        }
    }
    printf("P3\n%d %d\n255\n", maze->size_x, maze->size_y);
    for (i = 0; i < maze->size_x * maze->size_y; i++) {
        if (maze->bludiste[i] == WALL) {
            printf("255 0 0 ");
        }
        else {
            printf("0 %d 0 ", maze->bludiste[i]);
        }
    }
}

int main() {
    int S, W, H, P, C;
    int X, Y;
    int i, j;

    scanf("%d%d%d%d%d", &S, &W, &H, &P, &C);
    blud mesto = {W, H, NULL};
    mesto.bludiste = calloc(sizeof(int), W*H);
    if (mesto.bludiste == NULL) {
        puts("Málo paměti");
        exit(1);
    }
    int route[C][4];
    switch (S) {
        case 1:
            fav = city1;
            break;
        case 2:
            fav = city2;
            break;
        case 3:
            fav = city3;
            break;
        case 4:
            fav = city4;
            break;
    }
    for (; P > 0; P--) {
        scanf("%d%d", &X, &Y);
        mesto.bludiste[X + Y*W] = WALL;
    }
    for (i = 0; i < C; i++) {
        for (j = 0; j < 4; j++) {
            scanf("%d", &route[i][j]);
        }
    }
    for (i = 0; i < C; i++) {
        solve(&mesto, route[i]);
    }
    color(&mesto);
    return 0;
}
