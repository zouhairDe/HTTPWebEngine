#include <algorithm>
#include <climits>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <time.h>
#include <cmath>
#include <set>
#include <sstream>
#include <signal.h>
#include <fstream>
#include <istream>
#include <ostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <sys/sendfile.h>

// # define PORT 8888
// # define MAX_EVENTS 1024
// # define BUF_SIZE 1024
// # define MAXLINE 80
// # define MAX_EVENTS 1024
// # define INADDR_ANY ((unsigned long int) 0x00000000)
// # define EPOLL_CTL_ADD 1

using namespace std;

int main()
{
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1)
	{
		perror("socket error");
		exit(1);
	}

	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
	if (ret == -1)
	{
		perror("bind error");
		exit(1);
	}

	ret = listen(lfd, 64);
	if (ret == -1)
	{
		perror("listen error");
		exit(1);
	}

	int epfd = epoll_create(3000);
	if (epfd == -1)
	{
		perror("epoll_create error");
		exit(1);
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

	struct epoll_event all[1];
	while (1)
	{
		int ret = epoll_wait(epfd, all, 1, -1);
		if (ret == -1)
		{
			perror("epoll_wait error");
			exit(1);
		}
		else
		{
			cout << "epoll _wait ret: " << ret << endl;
		}

		for (int i = 0; i < ret; i++)
		{
			int fd = all[i].data.fd;
			if (fd == lfd)
			{
				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				int cfd = accept(lfd, (struct sockaddr *)&client, &len);
				if (cfd == -1)
				{
					perror("accept error");
					exit(1);
				}

				char ip[64] = {0};
				cout << "new client ip: " << inet_ntop(AF_INET, &client.sin_addr.s_addr, ip, sizeof(ip)) << ", port: " << ntohs(client.sin_port) << endl;

				ev.events = EPOLLIN;
				ev.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
			}
			else
			{
				if (!all[i].events & EPOLLIN)
				{
					continue;
				}

				char buf[5] = {0};
				int len = recv(fd, buf, sizeof(buf), 0);
				if (len == -1)
				{
					perror("recv error");
					exit(1);
				}
				else if (len == 0)
				{
					cout << "client closed" << endl;
					close(fd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				}
				else
				{
					cout << "recv buf: " << buf << endl;
					send(fd, buf, strlen(buf), 0);
				}
			}
		}	
	}
	
	close(lfd);
	close(epfd);
	//wtf is this shit
	
	return 0;
}