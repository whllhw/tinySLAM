//
// Created by lhw on 18-5-17.
//
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "recv_data.h"

class Server{

private:
    int server_sockfd;//服务器端套接字
    int client_sockfd;//客户端套接字
    bool Server_open;
    void setup(){
    int len;
    struct sockaddr_in my_addr;   //服务器网络地址结构体
    struct sockaddr_in remote_addr; //客户端网络地址结构体
    int sin_size;
    memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零
    my_addr.sin_family=AF_INET; //设置为IP通信
    my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
    my_addr.sin_port=htons(1234); //服务器端口号

    /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
        perror("socket");
        return ;
    }
        /*将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
    {
        perror("bind");
        return ;
    }
    printf("listening...on port 1234\n");
    /*监听连接请求--监听队列长度为5*/
    listen(server_sockfd,1);

    sin_size=sizeof(struct sockaddr_in);

    /*等待客户端连接请求到达*/
    if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,(socklen_t *)&sin_size))<0)
    {
        perror("accept");
        return ;
    }
    printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
    Server_open = true;
}
public:
    Server(){
        setup();
    }
    ~Server(){
        shutdown(client_sockfd,SHUT_RDWR);
        shutdown(server_sockfd,SHUT_RDWR);
    }
    UnionData pull(){
//        static uint8_t buf[4096];
        UnionData unionData;
        int len = 0;
        static int size = sizeof(unionData);
        len = recv(client_sockfd,(void *)&unionData,size,0);
//        printf("recved: %d ,expected: %d\n",len,size);
//        printf("sizeof laser_data:%d,encoder_data:%d,uniondata:%d\n",sizeof(Laser_data),sizeof(Encoder_data),sizeof(UnionData));
        if(len == size){
            return unionData;
        }else {
            throw unionData;
        }
    }
};
static Server* server;

UnionData pull(){
    if(!server){
        server = new Server();
    }
    return server->pull();
}