
#include "PolicyManager.h"
#include "PolicyListener.h"
#include "Thread.h"

int main(int argc, char **argv) {
	PolicyManager manager;

	Thread::runThread(PolicyListener::run, &manager);

	manager.managePolicy();

	return 0;
}
