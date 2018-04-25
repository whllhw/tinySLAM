#include "bridge.h"
#include "READ_UART.h"
#include <stdio.h>

static DY *dy = NULL;
static LDS *lds = NULL;

// for cgo call
SCAN pull_scan(){
    if (lds == NULL){
        lds = new LDS();
    }
    return lds->pull();
}

Dynamixel pull_dy(){
    if(dy == NULL){
        dy = new DY();
    }
    return dy->pull();
}

void shutdown(){
    delete lds;
    delete dy;
}

void Test(){
    printf("hi,uart is ok");
}