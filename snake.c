//gcc snake.c -o snake $(pkg-config --libs allegro-5 allegro_primitives-5)
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "util.h"
#include <stdio.h>
#include <stdbool.h>

//Consts
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define CIMA 0
#define DIREITA 1
#define BAIXO 2
#define ESQUERDA 3
#define HEAD "HEAD"
#define BODY "BODY"

typedef struct LinkedList{
    int x;
    int y;
    int direction;
    struct LinkedList *next;
} LinkedList;

typedef struct Square{
    char *type;//HEAD or BODY
    int direction;
    int x;
    int y;
    LinkedList *nextMove;
} Square;

//CONFIG VARIABLES
const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;
//------------------

//OTHERS VARIABLES
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
LinkedList* lastNode = NULL;
//------------------

//GAME VARIABLES
int sizeSquare = 30;
int gapSquare = 3;//Gap between squares
int speed = 10;
//------------------

bool inicializar();
bool isOutOfBounds();
bool isAxisY();
bool isLastIndex(int index, int size);
void changePosition(Square *square, bool isLastSquare);
void correctPosition();
void drawSquare();
LinkedList* createNode(Square head, int direction);
void freeNodeRecursive(LinkedList *currentNode);
LinkedList* freeNodeGetNext(LinkedList *currentNode);

int main(){
    if(!inicializar()){
        return -1;
    }

    int x = randBelow(LARGURA_TELA);
    int y = randBelow(ALTURA_TELA);

    Square snake[] = {
        {HEAD, DIREITA, x, y, NULL},
        {BODY, DIREITA, x-gapSquare-sizeSquare, y, NULL}
    };

    bool shouldExit = false;
    int selectedDirection, directionBeforeChange;

    while(shouldExit == false){
        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.05);
        bool hasNewNode = false;
        int hasEvents = al_wait_for_event_until(fila_eventos, &evento, &timeout);
        selectedDirection = -1;//Initialize with a not defined value for direction every loop
        if(hasEvents){
            if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                directionBeforeChange = snake[0].direction;
                switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_UP:
                        selectedDirection = CIMA;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        selectedDirection = BAIXO;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        selectedDirection = ESQUERDA;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        selectedDirection = DIREITA;
                        break;
                }
                if(directionBeforeChange != selectedDirection){
                    LinkedList *nodeRef = createNode(snake[0], selectedDirection);
                    if(lastNode != NULL){
                        lastNode->next = nodeRef;
                    }
                    lastNode = nodeRef;//Replace for the newest lastNode
                    hasNewNode = true;
                }
            } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                shouldExit = true;
                freeNodeRecursive(snake[ARRAY_SIZE(snake)-1].nextMove);
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_flip_display();

        for(int i = 0; i < ARRAY_SIZE(snake); i++){
            if(directionBeforeChange != selectedDirection){//Prevent action when selectedDirection is the current direction
                if(hasNewNode && snake[i].type == BODY && snake[i].nextMove == NULL){
                    snake[i].nextMove = lastNode;
                }

                if(i == 0 && selectedDirection != -1){
                    snake[i].direction = selectedDirection;
                }
                changePosition(&snake[i], isLastIndex(i, ARRAY_SIZE(snake)));
            }
            correctPosition(&snake[i]);
            drawSquare(&snake[i]);
        }

        al_flip_display();
    }

    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

bool isOutOfBounds(int position, int direction){
    if(direction == DIREITA){
        return position > LARGURA_TELA;
    } else if (direction == BAIXO){
        return position > ALTURA_TELA;
    } else {
        return position < (-sizeSquare);//It doesn't matter the direction, If it's below 0 then it's out
    }
}

bool isAxisY(int direction){
    return direction == BAIXO || direction == CIMA;
}

void changePosition(Square *square, bool isLastSquare){
    if(square->nextMove != NULL){
        if(square->nextMove->x == square->x && square->nextMove->y == square->y){//In the change position
            square->direction = square->nextMove->direction;
            if(isLastSquare){
                square->nextMove = freeNodeGetNext(square->nextMove);
            } else {
                square->nextMove = square->nextMove->next;
            }
        }
    }
    if(isAxisY(square->direction)){
        square->y += square->direction == BAIXO ? speed : -speed;
    } else {
        square->x += square->direction == DIREITA ? speed : -speed;
    }
}

void correctPosition(Square *square){
    int direction = square->direction;//Direction that the square is going
    if(isAxisY(direction)){
        if(isOutOfBounds(square->y, direction)){
            square->y = direction == CIMA ? ALTURA_TELA : -sizeSquare;
        }
    } else{
        if(isOutOfBounds(square->x, direction)){
            square->x = direction == DIREITA ? -sizeSquare : LARGURA_TELA;
        }
    }
}

LinkedList* freeNodeGetNext(LinkedList *currentNode){
    //double free or corruption (fasttop)
    //Abortado (imagem do núcleo gravada)
    //FIXME: Tem alguma coisa errada ao liberar a memoria, provavelmente to liberando algo q n precisava liberar. %p p/ ponteiro.
    if(currentNode != NULL){
        LinkedList *nextNode = currentNode->next;
        free(currentNode);
        return nextNode;
    }
    return NULL;
}

void freeNodeRecursive(LinkedList *currentNode){
    if(currentNode != NULL){
        LinkedList *nextNode = currentNode->next;
        free(currentNode);
        freeNodeRecursive(nextNode);
    }
}

bool inicializar(){
    if(!al_init()){
        fprintf(stderr, "Falha ao inicializar o Allegro\n");
        return false;
    }

    if(!al_init_primitives_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_primitives.\n");
        return false;
    }

    if (!al_install_keyboard()){
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela){
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }

    al_set_window_title(janela, "Snake Game - By Daniel");

    fila_eventos = al_create_event_queue();
    if(!fila_eventos){
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    return true;
}

void drawSquare(Square *square){
    int x = square->x;
    int y = square->y;
    if(square->type == BODY){
        switch (square->direction) {
            case CIMA:
                y += gapSquare;//Shift down
                al_draw_filled_rectangle(x, y, x+sizeSquare, y+sizeSquare, al_map_rgb(0, 255, 0));
                break;
            case DIREITA:
                x -= gapSquare;//Shift left
                al_draw_filled_rectangle(x, y, x+sizeSquare, y+sizeSquare, al_map_rgb(0, 255, 0));
                break;
            case BAIXO:
                y -= gapSquare;//Shift up
                al_draw_filled_rectangle(x, y, x+sizeSquare, y+sizeSquare, al_map_rgb(0, 255, 0));
                break;
            case ESQUERDA:
                x += gapSquare;//Shift right
                al_draw_filled_rectangle(x, y, x+sizeSquare, y+sizeSquare, al_map_rgb(0, 255, 0));
                break;
        }
    } else {
        al_draw_filled_rectangle(x, y, x+sizeSquare, y+sizeSquare, al_map_rgb(0, 255, 0));
    }
}

bool isLastIndex(int index, int size){
    return index == (size - 1);
}

LinkedList* createNode(Square head, int direction){
    LinkedList *newNode = (LinkedList*) calloc(1,sizeof(LinkedList));
    newNode->x = head.x;
    newNode->y = head.y;
    newNode->direction = direction;
    newNode->next = NULL;
    return newNode;
}
