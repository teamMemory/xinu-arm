/**
* Author: Paul Aman
*/

#ifndef __MYSIGNAL_H_INCLUDED__
#define __MYSIGNAL_H_INCLUDED_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/signal.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

/**
* Handler for program exit signal
* signal_id - signal to handle
*/
void handler_function(int signal_id);

/**
* sets up signal_action to handle all signals except for SIGKILL AND SIGSTOP
* signal_action - struct that forwards signals to a handler
*/
void setupSignalAction(struct sigaction signal_action);

#endif
