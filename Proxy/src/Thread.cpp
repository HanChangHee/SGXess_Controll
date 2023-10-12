/*
 * Thread.cpp
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */

#include <pthread.h>
#include "Thread.h"

Thread::Thread() {
}

Thread::~Thread() {
}

bool Thread::runThread(void *(*run) (void *), void *argv) {
	pthread_t pThread;

	::pthread_create(&pThread, NULL, run, argv);
	::pthread_detach(pThread);

	return true;
}
