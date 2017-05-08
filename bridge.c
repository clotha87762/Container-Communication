#define _GNU_SOURCE
#include "./container_B/rootfs/msghelp.h"
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
#include <sys/inotify.h>

#define N_NS 3
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
char *ns_substr[N_NS] = {"net", "mnt", "ipc"};
int ns[N_NS] = {CLONE_NEWNET, CLONE_NEWNS, CLONE_NEWIPC};

int main(int argc, char *argv[])
{	
	
	if(argc == 3){
	
	// ./bridge /proc/containerA PID/ns/ipc /proc/containerB PID/ns/mnt
	
	if(!strstr(argv[2],"ipc")){
		printf("the first argument need to be the ipc ns directory of container A(client)\n");
		return 1;
	}
	if(!strstr(argv[1],"mnt")){
		printf("the second argument need to be the mnt directory of container B(server)\n");
		return 1;
	}
	
	
	if(setns(open(argv[2],O_RDONLY),CLONE_NEWIPC)){
		printf("setns with client fail\n");
		return 1;
	}
	if(setns(open(argv[1],O_RDONLY),CLONE_NEWNS)){
		printf("setns with server fail\n");
		return 1;
	}
	}
	else{
		//printf("111\n");
		char pid_name[100];
		FILE* pid = fopen("./pid_file/b.pid","r");
		fgets(pid_name,100,pid);
		char pathB[500] = "/proc/";
		strcat(pathB,pid_name);
		strcat(pathB,"/ns/ipc");
		//printf("222 %s\n",pathB);
		if(setns(open(pathB,O_RDONLY),CLONE_NEWIPC)){
                	printf("setns with client fail\n");
                	return 1;
        	}
		//printf("333\n");
		fclose(pid);
		pid = fopen("./pid_file/a.pid","r");
		fgets(pid_name,100,pid);
		char pathA[500] = "/proc/";
		strcat(pathA,pid_name);
		strcat(pathA,"/ns/mnt");
		//printf("444 %s\n",pathA);
        	if(setns(open(pathA,O_RDONLY),CLONE_NEWNS)){
                	printf("setns with server fail\n");
                	return 1;
        	}



	}

	struct msg_buf msg,msg2;
	int msgqid ,msgqid2, tmp1,tmp2 , length , infd ,wd ,flag;
	struct inotify_event* event;
	char readBuf[EVENT_BUF_LEN];
	char test[1000];
	
	
	msgqid = msgget(MAGIC,MSGPERM|IPC_CREAT);
	
	
	if(msgqid<0){
	  //printf("aaa\n");
	  msgqid = msgget(MAGIC,0);
	  if(msgqid<0){
	    printf("msgget error\n");
	    return 1;
	  }
	}

	msgqid2= msgget(MAGIC+1,MSGPERM|IPC_CREAT);


	if(msgqid2<0){
          //printf("aaa\n");
          msgqid2 = msgget(MAGIC+1,0);
          if(msgqid2<0){
            printf("msgget error\n");
            return 1;
          }
        }
	 infd = inotify_init();
                if(infd<0){
                        perror(strerror(errno));
                        printf("inotifyFd\n");
                        return 1;
                }
                wd = inotify_add_watch(infd,"/msg/",IN_CLOSE_WRITE);

                if (wd == -1) {
                        perror(strerror(errno));
                        printf("inotify_add_watch error\n");
                        return 1;
                }
	

	
	//msgqid = msgget(5566,MSGPERM|IPC_CREAT); // Shall we add IPC_EXCL?
	
	//printf("aaa\n");
	while(1){
		//msg.mtext[0] = 0;
		tmp1 = msgrcv(msgqid,&msg,sizeof(msg.mtext),0,0);
		if(tmp1 <0){
			perror( strerror(errno) );
			printf("msgrcv failed, rc=%d\n", tmp1);
			return 1;
		}
		// Send msg to server and recv response ,sg
		printf("recv from client:%s",msg.mtext);
		
		
		//msg.mtext[0] = 0;

	

		 FILE* fp = fopen("/msg/bridge_msg","w");

                //fwrite(&msg.mtext[0],sizeof(char),sizeof(msg.mtext),fp);
                fprintf(fp,"%s",msg.mtext);
                fclose(fp);

		
	

		flag = 0;
		//printf("oao\n");
		while(1){
			length = read(infd,readBuf,EVENT_BUF_LEN);
			if(length <0){
				perror(strerror(errno));
				printf("inotify read error\n");
				return 1;
			}
			int i;
			i=0;
			while(i<length){
				event = (struct inotify_event*) readBuf;
				if(event->mask | IN_CLOSE_WRITE && !strcmp(event->name,"server_msg")){
					FILE* fp = fopen("/msg/server_msg","r");
					char c,temp;
					char cBuf[50000];
					temp = 0;
					while( (c = fgetc(fp))!=EOF){
						cBuf[temp++] = c;
					}
					
					cBuf[temp] = '\0';
					
					strcpy(msg.mtext,cBuf);
					strcpy(test,cBuf);		
					system("rm -f /msg/server_msg");
					flag = 1;
					break;
				}	
				i += event->len + EVENT_SIZE;
			}
			if(flag){
				break;
			}

		}
		printf("recv from server:%s",test);
		tmp1 = msgsnd(msgqid2,&msg,sizeof(msg.mtext),0);
		if(tmp1 <0){
			perror( strerror(errno) );
			printf("msgrcv failed, rc=%d\n", tmp1);
			return 1;
		}
	}	

	
	return 0;
}
