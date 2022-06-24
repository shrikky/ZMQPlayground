#include <iostream>
#include <zmq_addon.hpp>
#include "zmq_utils.h"
#include <stdlib.h>
#include <chrono>
#include <zmq.hpp>
#include "zmq.h"
static zmq::context_t ctx;

int main(int argc, char *argv[])
{
	zmq::context_t context(1);

	// init subs
	zmq::socket_t* subscriber = new zmq::socket_t(context, ZMQ_SUB);
	zmq::socket_t* subscriber2 = new zmq::socket_t(context, ZMQ_SUB);
	zmq::socket_t* subscriber3 = new zmq::socket_t(context, ZMQ_SUB);

	subscriber->connect("tcp://localhost:5556");
	subscriber2->connect("tcp://localhost:5556");
	subscriber3->connect("tcp://localhost:5556");
	std::vector<zmq::socket_t*> sublist = {};
	sublist.push_back(subscriber);
	sublist.push_back(subscriber2);
	sublist.push_back(subscriber3);

	// init topics
	std::string topicA = "RELOAD";
	std::string topicB = "DI";
	std::string topicC = "STATE_CONTROL";

	// set socket ops
	zmq_setsockopt(*subscriber,ZMQ_SUBSCRIBE, topicA.data(), topicA.size());
	zmq_setsockopt(*subscriber2, ZMQ_SUBSCRIBE, topicB.data(), topicB.size());
	zmq_setsockopt(*subscriber3, ZMQ_SUBSCRIBE, topicC.data(), topicC.size());

	// init poll
	zmq::pollitem_t pollers[] = {
		{ *subscriber,0,ZMQ_POLLIN,0},
		{ *subscriber2,0,ZMQ_POLLIN,0 },
		{ *subscriber3,0,ZMQ_POLLIN,0 },
	};
	//  Initialize random number generator
	srand((unsigned)time(NULL));

	while (1) {
		zmq::poll(pollers, 3, std::chrono::duration<long>(50));
		zmq::message_t topic;
		zmq::message_t payload;

		// Reload DI State
		for (int i = 0; i < 3; i++) {
			if (pollers[i].revents)
			{
				bool sendMore = true;
				while (sendMore) {
					sublist[i]->recv(topic, zmq::recv_flags::dontwait);
					if (topic.more()) {
						sublist[i]->recv(payload, zmq::recv_flags::dontwait);
					}
					if (!payload.more()) sendMore = false;
					std::cout << topic.to_string() << " " << payload.to_string() << std::endl;
				}
			}
		}
	}
	return 0;
}