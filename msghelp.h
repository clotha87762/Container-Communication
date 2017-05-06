#define MSGPERM 0600    
#define MSGTXTLEN 10000 
#define MAGIC 5566

struct msg_buf {
	long mtype;
	char mtext[MSGTXTLEN];
};
