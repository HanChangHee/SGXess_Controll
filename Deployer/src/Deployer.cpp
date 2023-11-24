#include <stdio.h>

#include "PolicyManager.h"
#include "PolicyListener.h"
#include "Thread.h"

int main(int argc, char **argv) {
	PolicyManager manager;

	clock_t start = clock();
	fflush(stdout);
	printf("run deployer");
	Thread::runThread(PolicyListener::run, &manager);

	clock_t end = clock();
	fflush(stdout);
	printf("time to run policy manager: %f sec \n", (double)((double)end - start) / CLOCKS_PER_SEC);
	manager.managePolicy();

	return 0;
}
