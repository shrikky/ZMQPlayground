#include <thread>
#include <chrono>
#include <iostream>
#include <zmq_addon.hpp>
#include <stdlib.h>
#include <zmq.h>
#include <string>

static zmq::context_t ctx;
using namespace std::chrono_literals;
int main(int argc, char *argv[])
{

	zmq::context_t context(1);
	zmq::socket_t publisher(context, ZMQ_PUB);

	//  Initialize random number generator
	srand((unsigned)time(NULL));

	publisher.bind("tcp://*:5556");

	//  Ensure subscriber connection has time to complete
	std::this_thread::sleep_for(std::chrono::seconds(1));

	//  Send out all 1,000 topic messages
	std::vector<std::string> topics = { "RELOAD","DI","STATE_CONTROL" };

	int iter = 0;
	while (1) {
		iter++;
		for (int i = 0; i < 3; i++) {
			auto r = rand() % 3 + 0;
			auto t = topics[r];
			zmq::message_t topic(t);
			std::string p = t;
			zmq::message_t payload(std::to_string(iter)); // send random data
			std::cout << "Topic:" << t << std::endl;
			publisher.send(topic, zmq::send_flags::sndmore);
			publisher.send(payload, zmq::send_flags::none);
		}
		std::this_thread::sleep_for(2000ms);
		std::cout << "-------------------------------------------" << std::endl;
	}
	return 0;
}