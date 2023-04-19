#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
FILE * fp = NULL;
int tcp_socket=0;
void my_handler (int param)
{
    if(fp)fclose(fp);
    if(tcp_socket)close(tcp_socket);
    printf("my_handler\r\n");

}
int main(int argc, char const *argv[])
{
    if(argc < 2)
    {
        printf("请输入发送的文件\n");
        return -1;
    }
    char file_msg[1024] = {0};
    signal (SIGINT, my_handler);
    size_t send_size=0;
    //获取文件大小
    struct stat file_size;

    stat(argv[1], &file_size);
    //拼接协议
    sprintf(file_msg, "file %s %ld",argv[1], file_size.st_size);
    printf("file %s %ld ",argv[1], file_size.st_size);

    if(file_size.st_size<=0)
    {
        return  0;
    }

    //发送文件名与大小给服务器  file 文件名 文件大小
    fp=fopen(argv[1], "rb");

    if(fp==NULL)
    {
        perror("");
        return -1;
    }

    //1.创建服务器socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    //2.设置服务器信息
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;//IPV4
    ser_addr.sin_port = htons(6667) ;//采用大端序
    ser_addr.sin_addr.s_addr = inet_addr("192.168.1.235");//自动绑定本地网卡地址

    //链接服务器
    int ret = connect(tcp_socket,(struct sockaddr *)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
        perror("");
        return -1;
    }else{
        printf("链接服务器成功！\n");
    }

    //发送给服务器
    write(tcp_socket, file_msg, strlen(file_msg));

    //等待服务器应答
    char rec[1024] = {0};
    read(tcp_socket, rec, 1024);
    printf("rec%s\n",rec);
    if(strcmp(rec,"GOGOGO") == 0)
    {
        while (1)
        {
            char data[1024] = {0};
            int size = fread(data, 1,1024,fp);//1个size  1024个char
            if(size <= 0 )
            {
                printf("读取完毕\n");
                break;
            }

            send_size+=size;
            printf("send size=%d send_size=%ld\r\n",size,send_size);
            write(tcp_socket,data, size);
        }

    }

    //等待服务器接收完毕
    bzero(rec,1024);
    read(tcp_socket,rec,1024);

    if(strcmp(rec,"down_ok") == 0)
    {
        printf("关闭所有链接\n");
        close(tcp_socket);
        fclose(fp);
    }

    return 0;
}
