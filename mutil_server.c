#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<time.h>
 
void usger(char* s)
{
    printf("Please input:%s [in_addr] [port_addr]\n",s);
}
 
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        usger(argv[0]);
        exit(1);
    }
    //printf("server\n");
 
 
 
    //1 get socket fd
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(2);
    }
    printf("%d\n",sock);
 
 
 
    //2 bind fd for port
    struct sockaddr_in server_socket;
    struct sockaddr_in client_socket;
 
    bzero(&server_socket, sizeof(server_socket));
 
    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = inet_addr(argv[1]);
    server_socket.sin_port = htons(atoi(argv[2]));
 
    int bind_num = bind(sock, (struct sockaddr*)&server_socket, sizeof(struct sockaddr));
    if(bind_num < 0)
    {
        close(sock);
        perror("bind");
        exit(3);
    }
 
    //3  listen
    if(listen(sock, 10) < 0)
    {
        close(sock);
        perror("listen");
        exit(4);
    }
 
 
    //4 accept
    while(1)
    {
        int len = 0;
        int new_fd = accept(sock, (struct sockaddr*)&client_socket, &len);
        if(new_fd < 0)
        {
            continue;
        }
        char buf[1024];
        memset(buf, '\0',sizeof(buf));
        inet_ntop(AF_INET, &client_socket.sin_addr, buf, sizeof(buf));
        printf("get client ip :%s\n",buf);
 
 
        pid_t id  = fork();//第一次fork
        if (id < 0)
        {
            perror("perror");
 
        }
        else if (id == 0)
        {
            close(sock);//关闭不用的文件描述符
            pid_t id = fork();//第二此fork
            if (id < 0)
            {
                perror("fork");
                exit(5);
 
            }
            else if(id == 0)//让孙子进程去处理
            {
 
                while(1)
                {
                char buff[1024];
                memset(buff,0,sizeof(buff));
                ssize_t s = read(new_fd, buff, sizeof(buff)-1);
                buff[strlen(buff)-1] = '\0';
		        time_t timep;
		        struct tm *p;
		        time(&timep); 
	            p = gmtime(&timep);
                    if(s > 0)
                {
                    printf("client :# ");
                    fflush(stdout);
                    printf("%s\n", buff);
                    printf("server :# \n");
                   
                    if(strncasecmp(buff,"Show me the time!",17) == 0){
                        printf("Return the client time!\n");
                        time(&timep);
		    		    p = localtime(&timep);
    		    		strcpy(buff,asctime(p));
/*
 					strcat(buff,char(1900+p->tm_year));
 					strcat(buff,"/");
 					strcat(buff,char(1+p->tm_mon));
 					strcat(buff,"/");
 					strcat(buff,char(p->tm_mday));
*/                  
                  }
                    else{
                        printf("No request for time!\n");
                    }
                   
                        write(new_fd, buff, strlen(buff)+1);

                }
                else if(s == 0)
                {
                    printf("client close!\n");
                    break;
                }
                }
            }
            else
            {
                exit(6);//子进程直接退出
            }
        }
        else
        {
            close(new_fd);//关闭不用的文件描述符
            waitpid( id, NULL, 0);//等待子进程退出（子进程会立即退出）
        }
    }
    close(sock);
    return 0;
}
