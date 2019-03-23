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

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

//GAME VARIABLES
int sizeSquare = 30;
int gapSquare = 3;//Gap between squares
int speed = 10;
//------------------

bool inicializar();
bool isOutOfBounds();
bool isAxisY();
void correctPosition();
void drawSquare();

typedef struct Square{
    char *type;//HEAD or BODY
    int direction;
    int x;
    int y;
} Square;

int main(){
    if(!inicializar()){
        return -1;
    }

    int x = randBelow(LARGURA_TELA);
    int y = randBelow(ALTURA_TELA);

    Square snake[] = {
        {HEAD, DIREITA, x, y},
        {BODY, DIREITA, x-gapSquare-sizeSquare, y}
    };

    bool shouldExit = false;
    int selectedDirection = DIREITA;

    while(shouldExit == false){
        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.05);

        int hasEvents = al_wait_for_event_until(fila_eventos, &evento, &timeout);
        if(hasEvents){
            if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
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
            } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                shouldExit = true;
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_flip_display();

        for(int i = 0; i < ARRAY_SIZE(snake); i++){
            if(snake[i].direction != selectedDirection){
                snake[i].direction = selectedDirection;
                if(isAxisY(snake[i].direction)){
                    snake[i].y += snake[i].direction == BAIXO ? speed : -speed;
                } else {
                    snake[i].x += snake[i].direction == DIREITA ? speed : -speed;
                }
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
