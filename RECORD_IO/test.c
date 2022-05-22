#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

    char testbuf[30] = {0};
    char smaplebuf[8] = {0};

    memset(testbuf, 0 , sizeof(testbuf));
    memset(smaplebuf, 0, sizeof(smaplebuf));

    printf("%s \n", testbuf);

    return 0;
}