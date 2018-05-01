#include "bridge.h"
#include "READ_UART.h"
#include <cstdio>

#include <csignal>
#include <execinfo.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

/*
 * 考虑到 cgo 调用 c 开销比较大，
 * 而且程序是要运行在嵌入式上面的
 * 直接在C语言里进行数据处理，图形生成
 * 只回传生成的图像信息，位置信息
 */

static DY *dy;
static LDS *lds;

// static bool isShutdown;

// // 每个线程都会将数据放入这里
// static Dynamixel dy_data;
// static SCAN scan_data;

struct SCAN pull_scan(){
    if (lds == NULL){
//        printf("init0\n");
        lds = new LDS();
    }
//    printf("now in lds->pull()\n");
    return lds->pull();
}

struct Dynamixel pull_dy(){
    if(dy == NULL){
        // printf("init0\n");
        dy = new DY();
    }
    // printf("now in dy->pull()\n");
    return dy->pull();
}

void shutdown(){
    if(lds != NULL){
        delete lds;
        lds = NULL;
    }
    if(dy != NULL){
        delete dy;
        dy = NULL;
    }
}
// // 记录线程PID
// static pthread_t thread1;
// static pthread_t thread2;

// // 互斥锁，
// pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// 开始进程 
// void start(){
//     shutdown();
//     if(thread1 == 0){
//         int ret1 = pthread_create(&thread1,NULL,pull_dy,NULL);
//         if (ret1 != 0)
//             printf("create thread:[pull_dy] failed!\n");
//     }
//     if(thread2 == 0){
//         int ret2 = pthread_create(&thread2,NULL,pull_scan,NULL);
//         if (ret2 != 0)
//             printf("create thread:[pull_scan] failed!\n");
//     }
//     isShutdown = false;

// }



using namespace std;

int faultOp(){
    char* addrPtr = reinterpret_cast<char*>(0x1);
    cout << (*addrPtr) << endl;
}

int outFunc(int num){
    if (num > 2)
        outFunc(num-1);
    faultOp();
}


void handleCore(int signo){
    printf("Signal caught:%d\n", signo);
    char* stack[20] = {0};
    int depth = backtrace(reinterpret_cast<void**>(stack), sizeof(stack)/sizeof(stack[0]));
    if (depth){
        char** symbols = backtrace_symbols(reinterpret_cast<void**>(stack), depth);
        if (symbols){
            for(size_t i = 0; i < depth; i++){
                printf("===[%d]:%s\n", (i+1), symbols[i]);
            }
        }
        free(symbols);
    }

    //re-throw
    raise(SIGSEGV);
}
int main(){
    // 需要同时读取两个串口的数据并进行拼接成为一个 结构体
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = &handleCore;
    act.sa_flags |= SA_RESETHAND; //one-time only
    sigaction(SIGSEGV, &act, NULL);
    for(;;){
            pull_scan();
            printf("\n");
    }
}