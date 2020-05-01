#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void err_sys(const char* x) ;
static void sig_int(int signo); // interrupt from keyboard

int main(int argc, char const *argv[])
{
	sigset_t newmask, oldmask, pendmask;

	printf("%s\n", "START SIGNAL\n" );
	printf("%s\n", "Please press Ctrl + C \n");

	if(signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("Can't catch SIGINT");

	/*
	* Block SIGINT and save current signal mask.
	*/

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	sleep(5); /* SIGINT here will remain pending */

	if(sigpending(&pendmask) < 0)
		err_sys("sigpending error");
	if(sigismember(&pendmask, SIGINT))
		printf("%s\n", "\nSIGINT pending\n");

	/*
	* Restore signal mask which unblocks SIGQUIT.
	*/
	//if(sigprocmask(SIG_UNBLOCK, &newmask, NULL) < 0)
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
			err_sys("SIG_SETMASK error");

	printf("%s\n", "SIGINT unblocked\n");
	sleep(5);
	
	exit(0);

	return 0;
}

void sig_int(int signo)
{
	printf("%s\n", "Caught SIGINT\n");
	if(signal(SIGINT, SIG_DFL) == SIG_ERR)
		err_sys("Can't reset SIGINT");
}

void err_sys(const char* x) 
{ 
    perror(x); 
    exit(1); 
}