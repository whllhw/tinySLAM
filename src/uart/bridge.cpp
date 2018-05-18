#include "bridge.h"
#include "READ_UART.h"
#include <cstdio>

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
#include <sys/time.h>
#include "send_data.h"
/*
 * 考虑到 cgo 调用 c 开销比较大，
 * 而且程序是要运行在嵌入式上面的
 * 直接在C语言里进行数据处理，图形生成
 * 只回传生成的图像信息，位置信息
 */
static volatile int keepRunning = 1;
void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("get ctrl c\n");
        keepRunning = 0;
    }
}
/*
 * 得到数据传送的频率 
 * 带缓冲则会把以前的数据读取下来。
 * laser 和 encoder 的频率均为 5 hz
 */
// int main()
// {
//     timeval startTime, stopTime;
//     unsigned int count = 0;
//     long start;
//     DY dy;
//     FILE *fp;
//     fp = fopen("test-dy","w");
//     Encoder_data encoder_data;
//     // system("sleep 5");                   // 带有缓冲区
//     for (int i = 0; i < 100; i++)
//     {
//         count = 0;
//         gettimeofday(&startTime, NULL);
//         start = startTime.tv_sec * 1000 + startTime.tv_usec / 1000;
//         do
//         {
//             encoder_data = dy.pull();
//             count++;
//             gettimeofday(&stopTime, NULL);
//             fprintf(fp,"%hu %hu %f\n",encoder_data.lspeed,encoder_data.rspeed,encoder_data.angle);
//         } while (stopTime.tv_sec * 1000 + stopTime.tv_usec / 1000 - start <= 1000);
//         printf("#%d -> dy: startTime:%ld,stopTime:%ld,times:%d\n", i, start, stopTime.tv_sec * 1000 + stopTime.tv_usec / 1000, count);
//     }

    // count =0;

    // LDS lds;
    // // system("sleep 5");
    // for (int i = 0; i < 100; i++)
    // {
    //     count = 0;
    //     gettimeofday(&startTime, NULL);
    //     start = startTime.tv_sec * 1000 + startTime.tv_usec / 1000;
    //     do
    //     {
    //         lds.pull();
    //         count++;
    //         gettimeofday(&stopTime, NULL);
    //     } while (stopTime.tv_sec * 1000 + stopTime.tv_usec / 1000 - start <= 1000);
    //     printf("#%d -> lds: startTime:%ld,stopTime:%ld,times:%d\n", i, start, stopTime.tv_sec * 1000 + stopTime.tv_usec / 1000, count);
    // }

    // delete &lds;
    // system("sleep 3");
// }

unsigned long get_msec(timeval &t)
{
    return t.tv_sec * 100000 + t.tv_usec / 100000;
}
struct read_lds_params
{
    LDS *lds;
    Laser_data data;
};
void *read_lds(void *params)
{
    read_lds_params *p = (read_lds_params *)params;
    p->data = p->lds->pull();
    return NULL;
}
// 机器只能向前，和旋转（顺，逆）
// 向前，角度变化很小。速度会大于20
// 旋转时只变化角度。且速度小于20
// TODO:建立网络消息队列，实现类似ros的远程调试功能
// TODO:先完成直线的地图构建，旋转的需要判断。
int main()
{
    // 需要同时读取两个串口的数据并进行拼接成为一个 结构体
    signal(SIGINT, sig_handler);
    LDS lds;
    DY dy;
    timeval tv_encoder, stop_encoder, stop;
    // char buf[4000];
    int i = 0;
    float last_angle;
    unsigned long start;
    last_angle = dy.pull().angle; // 初始角度
    if (last_angle < 0)
        last_angle += 360;
    pthread_t thread1;
    int ret_thread1;
    read_lds_params params; // 给线程参数和返回值的地方。
    params.lds = &lds;
    Encoder_data encoderData;
    Laser_data *scan = &(params.data);

    // FILE *fp;
    gettimeofday(&stop, NULL);
    start = get_msec(stop);
    // sprintf(buf, "test-%ld", start);
    // fp = fopen(buf, "w");
    unsigned long tmp;
    int count = 0;
    for (; keepRunning; gettimeofday(&stop, NULL))
    { // TODO:目前频率 4Hz,有时会降到 2,3Hz。应该使用多线程/DMA?
        tmp = get_msec(stop);
        if (tmp - start >= 100000)
        {
            printf("\nstart: %lu stop: %lu count:%d\n", start, tmp, count);
            count = 0;
            start = tmp;
        }
        else
        {
            count++;
        }
        // 创建新的读取线程。4Hz
        ret_thread1 = pthread_create(&thread1, NULL, &read_lds, (void *)&params);

        if(ret_thread1 != 0){
            printf("faild to start read_lds thread!\n");
        }
        encoderData = dy.pull();     // 5Hz get speed pre 20 ms
                                     // 角度范围为 -180 ～ 180
                                     // 顺时针，角度增大
        pthread_join(thread1, NULL); // 等待读取完毕

        if (encoderData.angle < 0)
            encoderData.angle += 360;
        last_angle = encoderData.angle;
        push_sensor_data(encoderData,*scan,tmp);

        // fprintf(fp, "%lu %hu %hu %f ", tmp, encoderData.lspeed, encoderData.rspeed, encoderData.angle);
        // for (i = 0; i < 360; i++)
        // {
        //     fprintf(fp, "%hu ", scan->ranges[i]);
        // }
        // fprintf(fp, "\n");
    }
    setdown();
    printf("done!\n");
}

int testEncoder()
{
    signal(SIGINT, sig_handler);
    // LDS lds;
    struct timeval tv;
    DY dy;
    int i = 0;
    system("sleep 5");
    printf("start...\n");
    while (keepRunning)
    {
        Encoder_data enc = dy.pull();
        gettimeofday(&tv, NULL);
        printf("tv:%lu theta:%f lspeed:%hu rspeed:%hu\n", tv.tv_sec * 1000 + tv.tv_usec / 1000, enc.angle, enc.lspeed, enc.rspeed);
    }
}
