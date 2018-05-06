#include "bridge.h"
#include "READ_UART.h"
#include <cstdio>

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <pthread.h>

/*
 * 考虑到 cgo 调用 c 开销比较大，
 * 而且程序是要运行在嵌入式上面的
 * 直接在C语言里进行数据处理，图形生成
 * 只回传生成的图像信息，位置信息
 */

static DY *dy;
static LDS *lds;
static volatile int keepRunning = 1;

// static bool isShutdown;

// // 每个线程都会将数据放入这里
// static Dynamixel dy_data;
// static SCAN scan_data;

struct Laser_data pull_scan(){
    if (lds == NULL){
//        printf("init0\n");
        lds = new LDS();
    }
//    printf("now in lds->pull()\n");
    return lds->pull();
}

struct Encoder_data pull_dy(){
    if(dy == NULL){
        // printf("init0\n");
        dy = new DY();
    }
    // printf("now in dy->pull()\n");
    return dy->pull();
}

void shutdown(){
    keepRunning = 0;
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
void sig_handler( int sig )
{
     if ( sig == SIGINT)
    {
        printf("get ctrl c\n");
        shutdown();
    }
}
/*
 * 得到数据传送的频率 
 */
int getHz(){
    DY dy;
    time_t startTime,stopTime;
    unsigned int count = 0;
    system("sleep 5");
    time(&startTime);
    do{
        dy.pull();
        count++;
        time(&stopTime);
    }while(stopTime - startTime < 1);
    printf("dy: startTime:%ld,stopTime:%ld,times:%d\n",startTime,stopTime,count);
    count =0;

    LDS lds;

    system("sleep 5"); // waiting devices
    time(&startTime);
    do{
        lds.pull();
        count++;
        time(&stopTime);
    }while(stopTime - startTime < 1);
    printf("lds: startTime:%ld,stopTime:%ld,times:%d\n",startTime,stopTime,count);
}

#include <sys/time.h>
int main(){
    // 需要同时读取两个串口的数据并进行拼接成为一个 结构体
    signal( SIGINT, sig_handler);
    LDS lds;
    DY dy;
    time_t startTime,stopTime;
    system("sleep 5");// waiting devices
    struct timeval tv,stop;
    int q1=0,q2=0;
    int i=0;
    for(time(&startTime);stopTime < startTime + 60;time(&stopTime)) { // 60 sec
        gettimeofday(&tv,NULL);
        Laser_data scan = lds.pull();         // 
        Encoder_data encoderData = dy.pull(); // 5Hz get speed pre 20 ms
        gettimeofday(&stop,NULL);
        q1 += encoderData.lspeed * ((stop.tv_sec-tv.tv_sec)*1000 + (stop.tv_usec-tv.tv_usec)/1000);
        q2 += encoderData.rspeed * ((stop.tv_sec-tv.tv_sec)*1000 + (stop.tv_usec-tv.tv_usec)/1000);
        printf("%ld %d %d",stop.tv_sec,q1,q2);
        for(i=0;i<360;i++){
            printf("%d ",scan.ranges[i]);
        }
        printf("\n");
    }
    printf("done!\n");
}