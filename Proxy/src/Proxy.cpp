#include <stdio.h>
#include <string>

#include "Thread.h"
#include "ProxyListener.h"
#include "ProxyManager.h"

int main() {
	// Listener thread 실행
	ProxyManager manager;
	Thread::runThread(ProxyListener::run, &manager);

	manager.manageClient();

	/*
	Listener server;
	if ( false == server.init(30020) ) {
		printf("Failed to init server\n");
		return 0;
	}

	while(true) {
		int client = server.getClientConn();
		if ( -1 != client ) {
			std::string msg;
			SocketUtil::recv(client, msg);

			printf("[Recv msg]\n%s", msg.c_str());
			server.deleteClientConn(client);


			tlsclient.sendMsg(msg);
			tlsclient.terminate();
		}
		else {
			printf("Failed to get client conn\n");
		}
	}


	TLSServer server;

	server.setListener("30020");

	while ( true ) {
		if ( false == server.acceptClient() ) {
			printf("PolicyListener failed to get client\n");
			server.resetClient();
			continue;
		}

		Validator::validate(server.getSSL());

		server.resetClient();
	}
	*/

    return 0;
}
