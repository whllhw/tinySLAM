#include "bridge.h"
#include "READ_UART.h"
#include <stdio.h>

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
        lds = new LDS();
    }
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

void Test(){
    // 需要同时读取两个串口的数据并进行拼接成为一个 结构体

}