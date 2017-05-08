#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include<errno.h>
#include"msghelp.h"

int main(int argc,char* argv[]){
	
	char test[1000];
	int msgqid,msgqid2, rc;
	struct msg_buf msg,msg2;

	
	msgqid = msgget(MAGIC,IPC_CREAT|MSGPERM);
	

	if(1){
		//printf("delete queue 1\n");
		rc = msgctl(msgqid, IPC_RMID, NULL);
		if(rc <0){
			printf("can't delete msg queue\n");
		}
	}
	
	msgqid2 = msgget(MAGIC+1,IPC_CREAT|MSGPERM);

	if( 1){
		
		//printf("delete queue 2\n");
		rc = msgctl(msgqid2, IPC_RMID, NULL);
                if(rc <0){
                        printf("can't delete msg queue\n");
                }
	}



	  msgqid = msgget(MAGIC,MSGPERM|IPC_CREAT|IPC_EXCL);

                if(msgqid<0){
                  //printf("aaa\n");
                  msgqid = msgget(MAGIC,0);
                  if(msgqid<0){
                        perror(strerror(errno));
                        printf("failed to create message queue with msgqid = %d\n", msgqid);
                        return 1;
                  }
                }

                msgqid2 = msgget(MAGIC+1,MSGPERM|IPC_CREAT|IPC_EXCL);

                if(msgqid2<0){
                  //printf("aaa\n");
                  msgqid2 = msgget(MAGIC+1,0);
                  if(msgqid2<0){
                        perror(strerror(errno));
                        printf("failed to create message queue with msgqid = %d\n", msgqid);
                        return 1;
                  }
                }


	
	
	while(1){
		

				

		msg.mtype = 1;
	
		//printf("aa\n");	

		fgets(msg.mtext,1000,stdin);
		//printf("fget:%s ",msg.mtext);
		rc = msgsnd(msgqid, &msg, sizeof(msg.mtext), 0);
		if (rc < 0) {
			perror( strerror(errno) );
			printf("msgsnd failed, rc = %d\n", rc);
			return 1;
		}
		
		//printf("www:%s",msg2.mtext);
		rc = msgrcv(msgqid2,&msg2,sizeof(msg2.mtext),0,0);
		if(rc < 0){
			perror(strerror(errno));
			printf("msgrcv failed, rc = %d\n",rc);
			return 1;
		}
		printf("%s",msg2.mtext);
		//rc = msgctl(msgqid, IPC_RMID, NULL);
	//	if(rc <0){
	//	   printf("can't delete msg queue\n");
	//	}
	}

	return 0;
}
