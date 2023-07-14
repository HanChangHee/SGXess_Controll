#include <unistd.h>

#include "Thread.h"
//#include "ACListener.h"

#include "PolicyListener.h"
#include "Attestator.h"

#include "Policy.h"

int main(int argc, char **argv) {

	// 2. init AuthPolicy as static
	// 3. run listeners by master listener, with no thread

	Attestator attestator;

	if ( false == attestator.createCert() ) {
		printf("Failed to create cert\n");
	}
	else {
		printf("creating cert success\n");
	}


	//attestator.setRATLS();

	PolicyListener::run(NULL);

	return 0;
}

