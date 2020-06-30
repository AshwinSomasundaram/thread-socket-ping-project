#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;

#define MAXLINE 4096 /*max text line length*/

int
main(int argc, char **argv) 
{
	 int sockfd;
	 struct sockaddr_in servaddr;
	 char sendline[MAXLINE], recvline[MAXLINE];
	
	 string address = "129.110.92.15";
	 char addr[30];
	 strcpy(addr, address.c_str());
	 //basic check of the arguments
	 //additional checks can be inserted
	 if (argc !=2) {
		  perror("Usage: ./a4pingClient <port number>"); 
		  exit(1);
	 }
	
	 //Create a socket for the client
	 //If sockfd<0 there was an error in the creation of the socket
	 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
	  perror("Problem in creating the socket");
	  exit(2);
	 }
	 char port[20];
	 strcpy(port, argv[1]);
	 int SERV_PORT = atoi(port);
		
	 cout << "Step 2. Client Started with port " << SERV_PORT << endl;
	 cout << "\tthe client pid is " << getpid() << endl;
	 //Creation of the socket
	 memset(&servaddr, 0, sizeof(servaddr));
	 servaddr.sin_family = AF_INET;
	 servaddr.sin_addr.s_addr= inet_addr(addr);
	 servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
		
	 //Connection of the client to the socket 
	 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
	  perror("Problem in connecting to the server");
	  exit(3);
	 }
		
	cout << "Step 3. Client has connected to Server" << endl;
	 ifstream infile("a4ping2Data.txt");
	 cout << "Step 4. Client is ready to read the input file: a4ping2Data.txt" << endl;
	 string line;
	 while (getline(infile, line)) {
	 	line += '\0';
		 strcpy(sendline, line.c_str());
		 cout << "Step 5. Client - Input data is " << sendline << endl;
		 cout << ">>Client>> sending " << sendline << " to the server" << endl;
	 	 send(sockfd, sendline, strlen(sendline), 0);
			
		stringstream ping_command(line);
		string website;
		int count;
		ping_command >> website >> count;
		//if count is n, there are n+5 lines. we want to eat the first n+4 lines
		int rtt_message_received = -1;
		while(rtt_message_received == -1) {
			if(recv(sockfd, recvline, MAXLINE, 0) == 0) {
				perror("The server term prematoo");
				exit(4);
			}
			string message(recvline);

			rtt_message_received = message.find("rtt");
			if(rtt_message_received != -1) {
				
				message = message.substr(message.find("---"));
				strcpy(recvline, message.c_str());
		  		cout << ">>Client>> Summary of ping command\n" << recvline <<  endl;

			}
			
			//if the message we just got has an rtt. then rttmr != -1
		}

	 }
	
	 cout << "Step 12. End of the program" << endl;
	 exit(0);
}
