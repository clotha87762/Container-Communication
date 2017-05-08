#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int main(int argc, char *argv[])
{
	struct inotify_event * event;
	int infd,i,j,wd,length;
	char readBuf[50000];
	char msg[50000];
	system("rm -f /msg/server_msg");
	system("rm -f /msg/bridge_msg");
	infd = inotify_init();
	wd = inotify_add_watch(infd,"/msg/",IN_CLOSE_WRITE);
	i = 0;
	//printf("aaaa\n");
	while(1){
		i=0;
		length = read(infd,readBuf,EVENT_BUF_LEN);
		if(length<=0){
				perror(strerror(errno));
				printf("inotify read error\n");
				return 1;
		}
		//printf("bbbb\n");
		while(i<length){
			event = (struct inotify_event* ) readBuf;
			if(event->len&&event->mask|IN_CLOSE_WRITE&&!strcmp(event->name,"bridge_msg")){
				FILE* fp = fopen("/msg/bridge_msg","r");
				char c;
				int temp;
				temp = 0;
				while((c=fgetc(fp))!=EOF){
					msg[temp++] = c;
				}
				msg[temp] = '\0';
				printf("recv from bridge:%s",msg);
				system("rm -f /msg/bridge_msg");
				FILE* response = fopen("/msg/server_msg","w");
				fputs(msg,response);
				fclose(response);
				break;
			}
			i += EVENT_SIZE + event->len;
		}
		
		
	}

	return 0;
}
