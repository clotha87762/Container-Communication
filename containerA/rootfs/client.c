#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include<sys/errno.h>
#include<errno.h>
#include"msghelp.h"

int main(int argc,char* argv[]){

	int msgqid,msgqid2, rc;
	struct msg_buf msg,msg2;
	

	while(1){
		msgqid = msgget(5566,0);
		if (msgqid < 0) {
			perror(strerror(errno));
			printf("failed to create message queue with msgqid = %d\n", msgqid);
			return 1;
		}		

		msg.mtype = 1;
	
	

		scanf("%s", msg.mtext);
		printf("%s %d\n",msg.mtext,sizeof(msg.mtext));
		rc = msgsnd(msgqid, &msg, sizeof(msg.mtext), 0);
		if (rc < 0) {
			perror( strerror(errno) );
			printf("msgsnd failed, rc = %d\n", rc);
			return 1;
		}

		rc = msgrcv(msgqid,&msg,sizeof(msg.mtext),0,0);
		if(rc < 0){
			perror(strerror(errno));
			printf("msgrcv failed, rc = %d\n",rc);
			return 1;
		}
		printf("%s\n",msg.mtext);
	}

	return 0;
}