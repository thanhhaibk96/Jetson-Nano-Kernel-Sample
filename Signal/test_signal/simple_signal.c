#include <stdio.h>
#include <signal.h>

void my_signal_handler(int signo)
{
	printf("%s\n", "My handler is calling\n");
}

void main(int argc, char const *argv[])
{
	signal(SIGINT, my_signal_handler);
	while(1);
	
	return 0;
}