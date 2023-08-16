/*
 * Listener.h
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */

#ifndef LISTENER_H_
#define LISTENER_H_

class Listener {
public:
	Listener();
	virtual ~Listener();

	bool init(unsigned short port);
	int getClientConn();
	void deleteClientConn(int clientSock);

private:
	int m_sock;
};

#endif /* LISTENER_H_ */
