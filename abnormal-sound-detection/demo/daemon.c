#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>

#define error_exit(_errmsg_) \
	do { 		\
		perror(_errmsg_);\
		exit(EXIT_FAILURE);\
	}while (0)

int main()
{
	pid_t pid;
	int maxfileno;
	int fd;

	if (-1 == (pid = fork()))
		error_exit("fork");
	if (pid > 0) 
		exit(EXIT_SUCCESS);

	if (-1 == setsid())
		error_exit("setsid");
	chdir("/");
	umask(0);

	maxfileno = getdtablesize();
	for (fd = 0; fd < maxfileno; fd ++)
		close(fd);

	while (1){
		puts("this is a daemon !!");
		system("date >> /home/james/log.info");
		sleep(1);
	}

	return 0;
}
