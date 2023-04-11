#include <unistd.h>

#include "Thread.h"
#include "ACListener.h"
#include "PolicyListener.h"

int main(int argc, char **argv) {

	// 1. run as daemon
//	int ret = fork();
//	if ( getpid() == ret ) {
//		return 0;
//	}
//	else if ( -1 == ret ) {
//		return 0;
//	}

	// 2. init AuthPolicy as static
	// 3. run listeners by master listener, with no thread
	Thread::runThread(ACListener::run);

	Thread::runThread(PolicyListener::run);

	while(true) { sleep(1); }

	return 0;
}

