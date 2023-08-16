/*
 * Thread.h
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */

#ifndef THREAD_H_
#define THREAD_H_

class Thread {
public:
	Thread();
	virtual ~Thread();

	static bool runThread(void *(*run) (void *));
};

#endif /* THREAD_H_ */
