
#include <stdio.h>
#include <fstream>
#include <iostream>

#include "PolicyDeployer.h"

int main() {
	std::string policy;


	std::ifstream openFile("policy.txt");

	std::string line;
	while(std::getline(openFile, line)) {
		policy.append(line).append("\n");
	}
	openFile.close();

	printf("[Deploying Policy]\n");
	printf("%s", policy.c_str());
	printf("Policy Length : %d\n", policy.size());

	PolicyDeployer::deployPolicy(policy, "192.168.0.5", 30001);

	return 0;
}
