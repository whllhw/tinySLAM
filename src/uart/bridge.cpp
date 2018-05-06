#include "bridge.h"
#include "READ_UART.h"
#include <cstdio>

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

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
 * 带缓冲则会把以前的数据读取下来。
 * laser 和 encoder 的频率均为 5 hz
 */
int getHz(){
    timeval startTime,stopTime;
    unsigned int count = 0;
    long start;
    // DY dy;
    // system("sleep 5");                   // 带有缓冲区
    // for(int i=0;i<10;i++){
    // count = 0;
    // gettimeofday(&startTime,NULL);
    // start = startTime.tv_sec*1000 + startTime.tv_usec/1000;
    // do{
    //     dy.pull();
    //     count++;
    //     gettimeofday(&stopTime,NULL);
    // }while(stopTime.tv_sec*1000+stopTime.tv_usec / 1000 - start <= 1000);
    // printf("#%d -> dy: startTime:%ld,stopTime:%ld,times:%d\n",i,start,stopTime.tv_sec*1000+stopTime.tv_usec / 1000,count);
    // }

    // count =0;

    LDS lds;
    // system("sleep 5");
    for(int i=0;i<10;i++){
    count = 0;
    gettimeofday(&startTime,NULL);
    start = startTime.tv_sec*1000 + startTime.tv_usec/1000;
    do{
        lds.pull();
        count++;
        gettimeofday(&stopTime,NULL);
    }while(stopTime.tv_sec*1000+stopTime.tv_usec / 1000 - start <= 1000);
    printf("#%d -> lds: startTime:%ld,stopTime:%ld,times:%d\n",i,start,stopTime.tv_sec*1000+stopTime.tv_usec / 1000,count);
    }

    // delete &lds;
    // system("sleep 3");
}

long get_msec(timeval &t){
    return t.tv_sec*1000+t.tv_usec/1000;
}
// 机器只能向前，和旋转（顺，逆）
// 向前，角度变化很小。速度会大于20
// 旋转时只变化角度。且速度小于20
int main(){
    // 需要同时读取两个串口的数据并进行拼接成为一个 结构体
    signal( SIGINT, sig_handler);
    LDS lds;
    DY dy;
    timeval tv_encoder,stop_encoder,stop;
    int q1=0,q2=0;
    int i=0;
    float last_angle;
    long start;
    last_angle = dy.pull().angle;        // 初始角度
    if (last_angle < 0) last_angle += 360;
    gettimeofday(&stop,NULL);
    start = get_msec(stop);
    long tmp;
    int count = 0;
    for(;keepRunning;gettimeofday(&stop,NULL)) { // 目前频率 4Hz,有时会降到 2,3Hz。应该使用多线程。
        tmp = get_msec(stop);
        if (tmp - start >= 1000){
            printf("\nstart: %ld stop: %ld count:%d\n",start,tmp,count);
            count = 0;
            start = tmp;
        }else{
            count ++;
        }
        gettimeofday(&tv_encoder,NULL);
        Laser_data scan = lds.pull();
        Encoder_data encoderData = dy.pull(); // 5Hz get speed pre 20 ms
                                            // 角度范围为 -180 ～ 180
                                            // 顺时针，角度增大
        gettimeofday(&stop_encoder,NULL);
        if (encoderData.angle < 0) encoderData.angle += 360;
        if(abs(last_angle - encoderData.angle) < 1.0){ // 可以认为是向前的
            q1 += encoderData.lspeed * ((stop_encoder.tv_sec-tv_encoder.tv_sec)*1000 + (stop_encoder.tv_usec-tv_encoder.tv_usec)/1000);
            q2 += encoderData.rspeed * ((stop_encoder.tv_sec-tv_encoder.tv_sec)*1000 + (stop_encoder.tv_usec-tv_encoder.tv_usec)/1000);
        }else if(encoderData.angle - last_angle > 0 || (last_angle > 270 && encoderData.angle < 90)){ // 角度增大,顺时针
            q1 += encoderData.lspeed * ((stop_encoder.tv_sec-tv_encoder.tv_sec)*1000 + (stop_encoder.tv_usec-tv_encoder.tv_usec)/1000);
            q2 -= encoderData.rspeed * ((stop_encoder.tv_sec-tv_encoder.tv_sec)*1000 + (stop_encoder.tv_usec-tv_encoder.tv_usec)/1000);
        }else{ // 角度减小
            q1 -= encoderData.lspeed * ((stop_encoder.tv_sec-tv_encoder.tv_sec)*1000 + (stop_encoder.tv_usec-tv_encoder.tv_usec)/1000);
            q2 += encoderData.rspeed * ((stop_encoder.tv_sec-tv_encoder.tv_sec)*1000 + (stop_encoder.tv_usec-tv_encoder.tv_usec)/1000);
        }
        last_angle = encoderData.angle;
        // printf("%lu %d %d lspeed:%hu rspeed:%hu ",get_msec(stop),q1,q2,encoderData.lspeed,encoderData.rspeed);
        // for(i=0;i<360;i++){
        //     printf("%hu ",scan.ranges[i]);
        // }
        // printf("\n");
        
    }
    printf("done!\n");
}

int testEncoder(){
    signal( SIGINT, sig_handler);
    // LDS lds;
    struct timeval tv;
    DY dy;
    int i=0;
    system("sleep 5");
    printf("start...\n");
    while(keepRunning){
        Encoder_data enc = dy.pull();
        gettimeofday(&tv,NULL);
        printf("tv:%lu theta:%f lspeed:%hu rspeed:%hu\n",tv.tv_sec*1000 + tv.tv_usec/1000,enc.angle,enc.lspeed,enc.rspeed);
    }
}
