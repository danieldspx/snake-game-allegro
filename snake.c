#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "util.h"
#include <stdio.h>
#include <stdbool.h>

#define CIMA 0
#define DIREITA 1
#define BAIXO 2
#define ESQUERDA 3

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

int sizeSquare = 30;
int speed = 10;

bool inicializar();
bool isOutOfBounds();
void correctPosition();

int main(){
    if(!inicializar()){
        return -1;
    }

    int head_x = randBelow(LARGURA_TELA);
    int head_y = randBelow(ALTURA_TELA);
    int direction = ESQUERDA;

    bool shouldExit = false;

    while(shouldExit == false){
        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.05);

        int hasEvents = al_wait_for_event_until(fila_eventos, &evento, &timeout);
        if(hasEvents){
            if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_UP:
                        direction = CIMA;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        direction = BAIXO;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        direction = ESQUERDA;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        direction = DIREITA;
                        break;
                }
            } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                shouldExit = true;
            }
        }

        if(direction == CIMA || direction == BAIXO){
            head_y += direction == BAIXO ? speed : -speed;
        } else {
            head_x += direction == DIREITA ? speed : -speed;
        }

        correctPosition(&head_x, &head_y, direction);


        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_flip_display();
        al_draw_filled_rectangle(head_x, head_y, head_x+sizeSquare, head_y+sizeSquare, al_map_rgb(0, 255, 0));
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

void correctPosition(int *x, int *y, int direction){
    if(direction == CIMA || direction == BAIXO){
        if(isOutOfBounds(*y, direction)){
            *y = direction == CIMA ? ALTURA_TELA : -sizeSquare;
        }
    } else{
        if(isOutOfBounds(*x, direction)){
            *x = direction == DIREITA ? -sizeSquare : LARGURA_TELA;
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
