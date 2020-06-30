
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctime>
#include <signal.h>
#include <vector>
using namespace std;

char program[80];
char *args[100];

int fds[2];


void *ThreadPing(void *threadarg) {
	pid_t pid;

	if((pid = fork()) == 0) {
		close(fds[0]);
		dup2(fds[1], 1);
		int i = execvp(program, args);
		pthread_exit(NULL);
	}
		pthread_exit(NULL);
}
int main()
{
	int status;
	pid_t my_pid = getpid();
	cout << "Step 1. a4ping1 process starts. pid=" << my_pid << endl;
	ifstream file("a4ping1Data.txt");
	string l;
	string ping = "ping";
	string c = "-c";
	strcpy(program, ping.c_str());
	args[0] = (char *)ping.c_str();
	args[1] = (char *)c.c_str();
	args[4] = NULL;
	cout << "Step 2. Ready to read the input file: a4ping1Data.txt" << endl;
	while(getline(file, l) ){
		if(l == "") { continue; }
		string address = l.substr(0, l.find(' '));
		string n = l.substr(1 + l.find(' '), l.rfind(' ')-l.find(' ')-1);
		int num_child = atoi(l.substr(1 +l.rfind(' ')).c_str());
		
		args[2] = (char *)n.c_str();
		args[3] = (char *)address.c_str();
		printf("Step 3. Input data is %s %s %d\n", address.c_str(), n.c_str(), num_child);
		pipe(fds);
		
		
		pthread_t threads[5];
		int rc;
		int i;
		cout << "Step 4. Create Threads" << endl;
		for( i = 0; i < num_child; i++) {
			cout << "\tThread " << (i+1) << "(tid=100" << (i+1) << ") to run ping -c " << n << " " << address << endl;
			rc = pthread_create(&threads[i], NULL, ThreadPing, (void *)&i);
			if(rc) {
				cout << "Error: unable to create thread, " << rc << endl;
				exit(-1);
			}
		}
		//wait for all children to finish before trying to read from the pipe
		for(int i = 0; i < num_child; i++) {
			pthread_join(threads[i], NULL);
		}
		cout << "Step 5. All Threads are terminated. The parent processing the results" << endl;


			close(fds[1]);
			dup2(fds[0], 0);

			int ch;
			string out = "";
			printf("Processing results for %s\n", address.c_str());

			while((ch = getc(stdin)) != EOF) {
				out += (char)ch;
				if((char)ch == '\n') {
					if(out.find("---") != -1 || out.find("rtt") != -1) {
					cout << out;

					}
					out = "";
				}
				if(ferror(stdin))
				{ fprintf(stderr, "read error\n"); return -1; }
			}
	}
	file.close();
	cout << "Step 6. End of the program run" << endl;
	return 0;
}
