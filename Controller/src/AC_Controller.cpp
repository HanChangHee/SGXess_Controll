#include <unistd.h>

#include "Thread.h"

#include "PolicyManager.h"
#include "PolicyListener.h"
#include "RequestListener.h"

int main(int argc, char **argv) {
	// init policy manager > all thread share manager object pointer
	PolicyManager manager;

	// run policy listener
	//Thread::runThread(PolicyListener::run, &manager);

	// run request listener
	Thread::runThread(RequestListener::run, &manager);

	// main process: check policy update
	manager.managePolicy();

	return 0;
}

