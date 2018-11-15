#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <ctype.h>

#define SERV_PORT 9096	//服务端口
#define CLIENT_MAX 1024	//客户端最大连接


int main(int argc, char* argv[]){
	int listenfd, connfd, efd;
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	char buf[BUFSIZ];
	int n, i, j, nready, opt;
	struct epoll_event op[CLIENT_MAX+1], ep;

	//创建监听套接字
	listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//设置端口复用
	opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//绑定地址族
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//设置连接同时最高上限
	listen(listenfd, SOMAXCONN);

	//创建epoll
	efd = epoll_create(CLIENT_MAX+1);

	//添加请求连接监听套接字
	ep.events = EPOLLIN;
	ep.data.fd = listenfd;
	epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &ep);

	for(;;){
		//监听事件
		nready = epoll_wait(efd, op, CLIENT_MAX + 1, -1);
		for(i = 0; i < nready; i++){
			if(!(op[i].events & EPOLLIN))
				continue;
			//当有连接请求
			if(op[i].data.fd == listenfd){
				clie_addr_len = sizeof(clie_addr);
				//获取连接
				connfd = accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
				printf("%s:%d connect successfully!\n", inet_ntoa(clie_addr.sin_addr), ntohs(clie_addr.sin_port));
				//监听读事件
				ep.events = EPOLLIN;
				ep.data.fd = connfd;
				//添加至监听
				epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &ep);
			}else{	//客户端消息
				bzero(buf, sizeof(buf));
				n = read(op[i].data.fd, buf, sizeof(buf));
				//客户端关闭连接
				if(0 == n){
				    //从监听中移除
				    epoll_ctl(efd, EPOLL_CTL_DEL, op[i].data.fd, NULL);
				    //关闭连接
				    close(op[i].data.fd);
				}else{

					//转为大写
					for(j = 0; j < n; j++){
						buf[j] = toupper(buf[j]);
					}
					printf("Client ---------: %s \n",buf);
					//回写给客户端
					write(op[i].data.fd, buf, n);
				}
			}
		}
	}
	close(listenfd);
	close(efd);
	return 0;
}