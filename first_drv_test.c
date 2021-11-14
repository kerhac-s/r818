#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	int fd1,fd2;
	int val=1;
	fd1 = open("/dev/xyz",O_RDWR);
	if(fd1<0)
		printf("cat not open %d!\n",fd1);
	if(argc!=2)
	{
		printf("Usage:\n");
		printf("%s <on|off>\n",argv[0]);
		return 0;
	}
	
	if(strcmp(argv[1], "on")==0)
	{
		printf("led on.......\n");
		val=1;
	}
	
	else
	{
                printf("led off .....\n");
		val=0;

	}
	write(fd1,&val,4);
	return 0;
}
