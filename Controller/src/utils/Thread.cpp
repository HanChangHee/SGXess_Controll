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

bool Thread::runThread(void *(*run) (void *)) {
	pthread_t pThread;

	::pthread_create(&pThread, NULL, run, NULL);
	::pthread_detach(pThread);

	return true;
}
