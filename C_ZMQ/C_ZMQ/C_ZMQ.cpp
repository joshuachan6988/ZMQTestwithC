// C_ZMQ.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "zmq.h"
#include "pthread.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

static void* zeromq_thread_main(void* arg);

/* pthreads */
static pthread_t pid;
static pthread_mutex_t lock;

/* flags */
static bool initialized = false;

int _tmain(int argc, _TCHAR* argv[])
{
	
	//  Socket to talk to clients
	void *context = zmq_ctx_new();
	void *responder = zmq_socket(context, ZMQ_REP);
	int rc = zmq_bind(responder, "tcp://*:1234");
	assert(rc == 0);

	pthread_mutex_init(&lock, NULL);
	pthread_create(&pid, NULL, &zeromq_thread_main, NULL);

	while (true)
	{
		char buffer[30] = {'\0'};
		zmq_recv(responder, buffer, 30, 0);
		printf("Main Function Received String %s\n", buffer);

		if (!strcmp("Hello Main Function", buffer))
		{
			printf("Main Function Replying %s\n", "Hello Thread Function");
			printf("Size of data: %d\n", sizeof("Hello Thread Function"));
			zmq_send(responder, "Hello Thread Function", sizeof("Hello Thread Function"), 0);
			printf("Main Function ZMQ Send completed\n");
		}
		else if (!strcmp("Thread has Finished", buffer))
		{
			printf("Main Function Finished");
			break;
		}

		Sleep(1000);
		
	}


	return 0;
}

static void* zeromq_thread_main(void* arg)
{
	printf("Connecting to hello world server…\n");
	void *context = zmq_ctx_new();
	void *requester = zmq_socket(context, ZMQ_REQ);
	zmq_connect(requester, "tcp://localhost:1234");

	for (int i = 0; i < 10; i++) {
		char buffer[30] = {'\0'};
		printf("Thread Function Sending %s\n", "Hello Main Function");
		printf("Size of data: %d\n", sizeof("Hello Main Function"));
		zmq_send(requester, "Hello Main Function", sizeof("Hello Main Function"), 0);
		printf("Thread Function ZMQ Send completed\n");
		zmq_recv(requester, buffer, sizeof("Hello Thread Function"), 0);
		printf("Thread Function Received String %s\n", buffer);
	}

	zmq_send(requester, "Thread has Finished", sizeof("Thread has Finished"), 0);

	zmq_close(requester);
	zmq_ctx_destroy(context);

	return 0;
}


