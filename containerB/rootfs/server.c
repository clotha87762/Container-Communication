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
	infd = inotify_init();
	wd = inotify_add_watch(infd,getcwd(NULL,0),IN_CLOSE_WRITE);
	i = 0;
	printf("aaaa\n");
	while(1){
		length = read(infd,readBuf,EVENT_BUF_LEN);
		if(length<=0){
				perror(strerror(errno));
				printf("inotify read error\n");
				return 1;
		}
		printf("bbbb\n");
		while(i<length){
			event = (struct inotify_event* ) readBuf;
			if(event->len&&event->mask|IN_CLOSE_WRITE&&!strcmp(event->name,"bridge_msg")){
				FILE* fp = fopen("bridge_msg","r");
				char c;
				int temp;
				temp = 0;
				while((c=fgetc(fp))!=EOF){
					msg[temp++] = c;
				}
				msg[temp] = EOF;
				system("rm -f /tmp/bridge_msg");
				break;
			}
			i += EVENT_SIZE + event->len;
		}
		printf("cccc\n");
	        FILE* response = fopen("server_msg","w");
		fputs(msg,response);
		fclose(response);
		length = -1;
	}

	return 0;
}
