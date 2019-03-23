#include <time.h>
#include <stdlib.h>

int randBelow(int max){
    srand(time(NULL));
    return rand()%max;
}
