/*
 * Receiver.h
 *
 *  Created on: 2023. 4. 11.
 *      Author: chhan
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <string>

#define	MAX_BUF_LEN	256

class SocketUtil {
public:
	SocketUtil();
	virtual ~SocketUtil();

	static bool recv(int targetSock, std::string &data);
	static bool send(int targetSock, std::string &data);
};

#endif /* RECEIVER_H_ */
