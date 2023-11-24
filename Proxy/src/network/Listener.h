/*
 * Listener.h
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include <arpa/inet.h>

class Listener {
public:
	Listener();
	virtual ~Listener();

	bool init(unsigned short port);
	int getClientConn(sockaddr_in &client_addr);
	void deleteClientConn(int clientSock);

public:
	int m_sock;
};

#endif /* LISTENER_H_ */
