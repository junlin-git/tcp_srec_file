#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
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
        printf("请输入接收的文件\n");
        return -1;
    }
    signal (SIGINT, my_handler);
    //1.创建服务器socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    //2.绑定IP地址信息
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;//IPV4
    ser_addr.sin_port = htons(6667) ;//采用大端序
    ser_addr.sin_addr.s_addr = INADDR_ANY;//自动绑定本地网卡地址

    int ret = bind(tcp_socket,(struct sockaddr *)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
        perror("");
        return -1;
    }else{
        printf("绑定成功！\n");
    }
    //3.设置为监听模式
    listen(tcp_socket,5);

    //4.接收客户端链接
    for(int l=0;l<100;l++)
    {

        printf("等待客户端发送文件\r\n");
        int new_socket = accept(tcp_socket, NULL,NULL);


        if(new_socket > 0)
        {
            printf("开始接收...\r\n");
            //接收文件名+文件大小
            char file_msg[1024];//file 文件名 文件大小
            read(new_socket,file_msg,1024);


            //获取文件名和文件大小
            char file_name[1024];
            int file_size ;
            if(strstr(file_msg, "file"))
            {
                sscanf(file_msg, "file %s %d",file_name, &file_size);
            }else{
                printf("解析文件失败\r\n");
                close(new_socket);
                continue;
            }

            printf("对方发送的文件名:%s 文件大小%d\r\n",file_name,file_size);

            //告诉发送端，已经得到了文件的信息
            write(new_socket,"GOGOGO", strlen("GOGOGO"));

            //创建文件
            fp=fopen(argv[1], "wb+");
            //下载大小
            int dow_size = 0;
            //不断接收数据
            while (1)
            {
                //读取网络数据
                char data[4096] = {0};
                int size = read(new_socket,data,4096);
                dow_size += size;

                //写入本地文件
                int ret = fwrite(data,size,1,fp);
                if(ret <= 0){
                    fprintf(stderr, "fwrite failed\n");
                }
                //判断是否下载完毕
                if(dow_size >= file_size)
                {
                    printf("下载完毕 \r\n");

                    //告诉发送端以及下载完毕 可以断开连接
                    write(new_socket, "down_ok",strlen("down_ok"));
                    fclose(fp);
                    close(new_socket);

                    return 0;
                }else{
                    printf("下载进度 : %d  %%\n", dow_size*100/file_size);
                }

            }


        }

        usleep(1000);
    }

    return 0;
}
