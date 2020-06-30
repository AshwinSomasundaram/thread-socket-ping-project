#include <stdlib.h>

#include <fcntl.h>

#include <iostream>
#include <stdio.h>

#include <sys/types.h>

#include <sys/wait.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <sstream>
#include <string.h>

#include <unistd.h>

#include <string>
#include <time.h>

#include <signal.h>

using namespace std;
static int timer_expired = 0;

void Pinger(string website, string packets, int csock) {
	string ping = "ping";
	string dashc = "-c";
	
	char program[80];
	char* args[100];
	strcpy(program, ping.c_str());
	args[0] = (char *)ping.c_str();
	args[1] = (char *) dashc.c_str();
	args[2] = (char *)packets.c_str();
	args[3] = (char *)website.c_str();
	args[4] = NULL;
	pid_t pid;
	cout << "Step 8. Server forking Child Process" << endl;
	pid = fork();
	if(pid == 0) {
		cout << "\tchild process (pid=" << getpid() << ") to run ping -c " << packets << " " << website << endl;
		dup2(csock, STDOUT_FILENO);
		close(csock);
		execvp(program, args);
	} else {
//		close(csock);
		waitpid(pid, NULL, 0);
	}

}
static void alarm_handler(int sig) {
    timer_expired = 30;
    string t = "Server timed out";
    string e = "echo";
    char program[80];
    char* args[100];
    strcpy(program, e.c_str());
    args[0] = (char *)e.c_str();
    args[1] = (char *)t.c_str();
    args[2] = NULL;
    int i = execvp(program, args);
}


# define MAXLINE 4096 
/*max text line length*/ 
# define LISTENQ 8
/*maximum number of client connections */

int main(int argc, char ** argv) {
    //set up timeout signal
    //sigaction(SIGALRM, &(struct sigaction){.sa_handler = alarm_handler}, NULL);
    // time_t t;
    //srand((unsigned) time(&t));
    pid_t pid;
    pid = getpid();
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        return -1;
    }
    timer_expired = 0;
    alarm(120);
    int listenfd, connfd;
    socklen_t clilen;
    int n;
    char buf[MAXLINE];
    char reply[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    if (argc != 2) {
        printf("Usage: ./a4pingServer <port number> &\n");
        return -1;
    }

    char port[20];
    strcpy(port, argv[1]);
    int SERV_PORT = atoi(port);
    cout << "Step 1. Server Started with port " << SERV_PORT << endl;
    cout << "\tthe server pid is " << pid << endl;
    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr * ) & servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");


   for(;;) {
	
	clilen = sizeof(cliaddr);
	connfd = accept(listenfd, (struct sockaddr * ) & cliaddr, & clilen);
	            while ((n = recv(connfd, buf, MAXLINE, 0)) > 0) { // used to be && !timer_expired
	                if (n != 0) {
	                    strncpy(buf, buf, n);
	                    buf[n] = '\0';
	                    cout << ">>Server>> Received " << buf << " from the client" << endl;
			    stringstream ping_command(buf);
			    string website;
			    string packets;
			    ping_command >> website;
			    ping_command >> packets;
		            Pinger(website, packets, connfd);
			    cout << "Step 9. Server sent the result of the ping command to the client via socket" << endl;
	                }
		     }
	            //send(connfd, buf, n, 0);
	
	        if (n < 0) {
	            perror("Read error");
	            exit(1);
	        }
	        close(connfd);
    }

    //close listening socket
    close(listenfd);
    cout << "Client timed out" << endl;
    return 0;
}
