#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
    srand(time(NULL));

    int a = rand () % 15;

    for(int i = 0; i < 10; i++){
        
        printf("%d\n", a);
    }
}