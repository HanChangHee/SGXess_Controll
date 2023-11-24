#include <unistd.h>

#include <time.h>
#include "Thread.h"

#include "PolicyManager.h"
#include "PolicyListener.h"
#include "RequestListener.h"

int main(int argc, char **argv) {
	// init policy manager > all thread share manager object pointer
	clock_t start = clock();
	PolicyManager manager;

	// run policy listener
	Thread::runThread(PolicyListener::run, &manager);

	// run request listener
	Thread::runThread(RequestListener::run, &manager);

	clock_t end = clock();
	printf("time to run policy manager: %f sec \n", (double)((double)end - start) / CLOCKS_PER_SEC);
	// main process: check policy update
	manager.managePolicy();

	return 0;
}

