/**
* Author: Paul Aman
*/

#include "mySignal.h"

using namespace std;

/**
* Handler for all signals
* signal_id - signal to handle
*
* referenced signal.c given code
*/
void handler_function(int signal_id){
	// to do - free all memory
	fprintf(stdout, "\nHandling unknown signal: %d\n", signal_id);
	fflush(stdout);
}

/**
* sets up signal_action to SIGKILL
* signal_action - struct that forwards signals to a handler
*
* referenced project doc for setup
*/
void setupSignalAction(struct sigaction signal_action){
	signal_action.sa_handler = handler_function;
	signal_action.sa_flags = 0;
	sigemptyset( &signal_action.sa_mask );
	//if (i != SIGKILL && i != SIGSTOP && i != SIGQUIT){
	if (sigaction( SIGKILL, &signal_action, NULL ) < 0){ // is this the right signal?
		fprintf(stderr, "Signal Error: Setting up handler for signal: %i\n", i);
		fflush(stderr);
	}
}