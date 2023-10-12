#include <stdio.h>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include <string.h>

#include "SocketUtil.h"
#include "TLSClient.h"

int main() {
	unsigned int conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.5");
	server_addr.sin_port = htons(30020);

	if ( 0 != connect(conn, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		printf("Failed to connect\n");
		close(conn);
		return 0;
	}

	std::string msg = "gramine-ratls test message sended to proxy blahblahblah~\n";
	SocketUtil::send(conn, msg);

	close(conn);
	/*
	std::string request;
	TLSClient client;

	client.connect("", "");

	client.sendMsg(request);

	std::string response;
	client.recvMsg(response);
	client.terminate();

	printf("[Received Message]\n%s\n", response.c_str());
	*/

    return 0;

}
