#define MSGPERM 0600    
#define MSGTXTLEN 1024
#define MAGIC 5678

struct msg_buf {
	long mtype;
	char mtext[MSGTXTLEN];
};
