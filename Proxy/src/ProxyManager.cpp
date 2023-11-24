
#include <unistd.h>

#include "ProxyManager.h"

ProxyManager::ProxyManager() {
}

ProxyManager::~ProxyManager() {
}


void ProxyManager::insertClient(std::string client) {
	m_mutex.lock();

	m_clientList.push_back(client);

	m_mutex.unlock();
}

void ProxyManager::popClient(std::string &client) {
	m_mutex.lock();

	if ( false == m_clientList.empty() ) {
		client = m_clientList.front();
		m_clientList.pop_front();
	}

	m_mutex.unlock();
}


bool ProxyManager::manageClient() {
	if ( false == setTLSClient() ) {
		printf("Failed to make session with Controller.\n");
		return false;
	}

	while(true) {
		std::string client;
		popClient(client);

		if ( true == client.empty() ) {
			sleep(1);
			continue;
		}

		// send client ip data to Controller
		if ( false == m_tlsClient.sendMsg(client) ) {
			m_tlsClient.terminate();
			if ( false == setTLSClient() ) {
				printf("Failed to make session with Controller.\n");
				m_tlsClient.terminate();
				return false;
			}
			if ( false == m_tlsClient.sendMsg(client) ) {
				printf("Failed to send client IP to Controller. [IP] %s\n", client.c_str());
				m_tlsClient.terminate();
				return false;
			}
		}

		std::string msg;
		m_tlsClient.recvMsg(msg);
		printf("[IP] %s [RSP] %s\n", client.c_str(), msg.c_str());
	}

	return true;
}

bool ProxyManager::setTLSClient() {
	// Controller와 세션 생성
	if ( false == m_tlsClient.connect("192.168.0.5", "30011") ) {
		m_tlsClient.terminate();
		return false;
	}

	return true;
}

