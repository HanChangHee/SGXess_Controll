#include <unistd.h>

#include "Thread.h"
//#include "ACListener.h"

#include "PolicyListener.h"

#include "Policy.h"

int main(int argc, char **argv) {

	// 2. init AuthPolicy as static
	// 3. run listeners by master listener, with no thread

	PolicyListener::run(NULL);

	return 0;
}

