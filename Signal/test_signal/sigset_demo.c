#include <stdio.h>
#include <signal.h>


void main(int argc, char const *argv[])
{
	sigset_t my_sigset;
	
	sigemptyset(&my_sigset); // Clear all bits

	sigaddset(&my_sigset, SIGINT);

	sigprocmask(SIG_BLOCK, &my_sigset, NULL);

	while(1);

	return 0;
}