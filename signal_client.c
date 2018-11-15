#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
 
 
void usger(char* s)
{
    printf("Please input:%s [in_addr] [port_addr]\n",s);
}
 
int main(int argc, char* argv[])
{
    printf("1\n");
    if(argc != 3)
    {
        usger(argv[0]);
        exit(1);
    }
    //printf("client\n");
    //1.获得文件描述符
    int sock = socket(AF_INET, SOCK_STREAM, 0);
 
    struct sockaddr_in server_sock;
    server_sock.sin_family = AF_INET;
    server_sock.sin_port = htons(atoi(argv[2]));
    server_sock.sin_addr.s_addr = inet_addr(argv[1]);
 
    //2.连接服务器，此时调用connect相当于三次握手的第一次握手
    int ret = connect(sock, (struct sockaddr*)&server_sock, sizeof(server_sock));
 
    if(ret < 0)
    {
        perror("correct");
        return 1;
    }
    //发收数据
    while(1)
    {
        printf("client :# ");
        fflush(stdout);
        char buf[1024];
        int count = read(0, buf, sizeof(buf)-1);
        buf[count] = '\0';
        write(sock, buf, strlen(buf));
        read(sock, buf, sizeof(buf)-1);
        printf("server :# %s\n",buf);
    }
    close(sock);
    return 0;
}