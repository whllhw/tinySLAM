#include "bridge.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "thread_safe_queue.h"

using namespace std;
struct UnionData{
    struct Encoder_data e;
    struct Laser_data l;
    unsigned long time;
};

static gdface::mt::threadsafe_queue<UnionData> Data_queue;
static bool Network_status = false;
static int sock;
static pthread_t thread1;

void *Send_data_thread(void *d){
    printf("\nstarted Send_data\n");
    // 724 8 unsigned long 4 736
    printf("sizeof laser_data:%d,encoder_data:%d,uniondata:%d\n",sizeof(Laser_data),sizeof(Encoder_data),sizeof(UnionData));
    UnionData uniondata;
    int len = 0;
    int size = sizeof(uniondata);
    printf("%d\n",size);
    do{
        uniondata = Data_queue.wait_and_pop();
        // 发送失败，重新发送
        len = send(sock,(void *)&uniondata,size,0);
        if (len == -1){
            // printf("send data error");
            printf("send data error\n");
        }
        else if(len < size){
            // printf("send incomplete data");
            printf("send incomplete data :%d\n",len);
        }
    }while(true);
    return NULL;
}


// 作为客户端使用，发送数据到指定IP，端口
// IP:192.168.1.100
// PORT:1234
void setup(){
    if(!Network_status){
        sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
        serv_addr.sin_family = AF_INET;  //使用IPv4地址
        serv_addr.sin_addr.s_addr = inet_addr("192.168.1.100");  //具体的IP地址
        serv_addr.sin_port = htons(1234);  //端口
        int ret  = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        // 连接到服务器
        if(ret<0){
            printf("Network error! %d \n please retry!\n");
            return ;
        }
        else{
            Network_status = true;
            printf("Network done!");
        }
        // 创建数据发送的线程
        if(pthread_create(&thread1, NULL, &Send_data_thread,NULL) == 0){
            printf("start send_data thread1 success!\n");
        }else{
            printf("faild to start send_data thread1!\n");
        }
    }
    
}
void setdown(){
    if(thread1){
        char buff[] = "exit";
        pthread_cancel(thread1);            // 关闭发送进程
        send(sock,buff,sizeof(buff),0);
        shutdown(sock,SHUT_RDWR);           // 关闭套接字
        thread1 = 0;
    }
}
// void clear_sensor_queue(){
//     while(!Data_queue.empty())
//         Data_queue.pop();
// }

void push_sensor_data(Encoder_data e,Laser_data l,unsigned long time){
    if(!Network_status)
        setup();
    UnionData uniondata;
    uniondata.e = e;
    uniondata.l = l;
    uniondata.time = time;
    Data_queue.push(uniondata);
}
