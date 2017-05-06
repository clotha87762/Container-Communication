#define _GNU_SOURCE
#include "msghelp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <errno.h>
#include <limit.h>
#include <sys/inotify.h>

#define N_NS 3
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
char *ns_substr[N_NS] = {"net", "mnt", "ipc"};
int ns[N_NS] = {CLONE_NEWNET, CLONE_NEWNS, CLONE_NEWIPC};

int main(int argc, char *argv[])
{
	if(argc != 3){
		
		return 1;
	}
	
	// ./bridge /proc/containerA PID/ns/ipc /proc/containerB PID/ns/mnt
	
	if(!strstr(argv[1],"ipc")){
		printf("the first argument need to be the ipc ns directory of container A(client)\n");
		return 1;
	}
	if(!strstr(argv[2],"mnt"){
		printf("the second argument need to be the mnt directory of container B(server)\n";
		return 1;
	}

	if(!setns(open(argv[1],O_RDONLY),CLONE_NEWIPC)){
		printf("setns with client fail\n");
		return 1;
	}
	if(!setns(open(argv[2],O_RDONLY),CLONE_NEWNS)){
		printf("setns with server fail\n");
		return 1;
	}


	struct msg_buf msg;
	int msgqid , tmp1,tmp2 , length , infd ,wd ,flag;
	struct inotify_event* event;
	char readBuf[EVENT_BUF_LEN];
	msgqid = msgget(5566,0600|IPC_CREAT); // Shall we add IPC_EXCL?
	tmp1 = -1;
	tmp2 = -1;
	while(1){
		
		tmp1 = msgrcv(msgqid,&msg,sizeof(msg.mtext),0,0);
		if(tmp1 <0){
			perror( strerror(errno) );
			printf("msgrcv failed, rc=%d\n", rc);
			return 1;
		}
		// Send msg to server and recv response ,sg

		FILE* fp = fopen("/tmp/bridge_msg","w");
		fwrite(msg.mtext,sizeof(char),sizeof(msg.mtext),fp);
		fclose(fp);
		
		infd = inotify_init();
		if(infd<0){
			perror(strerror(errno));
			printf("inotifyFd\n");
			return 1;
		}
		wd = inotify_add_watch(infd,"/tmp/",IN_CLOSE_WRITE);
		
		if (wd == -1) {
			perror(strerror(errno));
			printf("inotify_add_watch\n");
			return 1;
		}
	

		flag = 0;

		while(1){
			length = read(infd,readBuf,EVENT_BUF_LEN);
			if(length <0){
				perror(strerror(errno));
				printf("inotify read error\n");
				return 1;
			}
			int i = 0;
			while(i<length){
				event = (struct inotify_event*) readBuf;
				if(event->mask | IN_CLOSE_WRITE && !strcmp(event->name,"server_msg")){
					FILE* fp = fopen("/tmp/mmsg","r");
					char c,temp;
					char cBuf[50000];
					temp = 0;
					while( (c = fgetc(fp))!=EOF){
						cBuf[temp++] = c;
					}
					cBuf[temp] = EOF;
					strcpy(msg.mtext,cBuf);
					system("rm -f /tmp/mmsg");
					flag = 1;
					break;
				}	
				i += event->len + EVENT_SIZE;
			}
			if(flag){
				break;
			}

		}

		
		tmp1 = msgsnd(msgqid,&msg,sizeof(msg.mtext),0);
		if(tmp1 <0){
			perror( strerror(errno) );
			printf("msgrcv failed, rc=%d\n", rc);
			return 1;
		}
	}	

	
	return 0;
}
