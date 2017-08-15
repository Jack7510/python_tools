// 
// test queue msg send & recv
// command line:  server dst_id src_id msg_len
//

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "msg.h"

int main( int argc, char* argv[] )
{
	int dst_task_id;
	int src_task_id;
	int msg_len;
	message_buf* send_buf;
	message_buf* recv_buf;
	int i;

	if( argc < 4 )
	{
		printf("usage: server server_id client_id len \n");
		return -1;
	}

	dst_task_id = atoi(argv[1]);
	src_task_id = atoi(argv[2]);
	msg_len 	= atoi(argv[3]);
	printf("server %d, cient %d, len %d\n", dst_task_id, src_task_id, msg_len );

	send_buf = (message_buf*) malloc( sizeof(message_buf) + msg_len );
	if( send_buf == NULL )
	{
		fprintf(stderr, "out of memory %d\n", __LINE__ );
		return -1;
	}

	recv_buf = (message_buf*) malloc( sizeof(message_buf) + msg_len );
	if( recv_buf == NULL )
	{
		fprintf(stderr, "out of memory %d\n", __LINE__ );
		return -1;
	}

	// create msg queues
	remove_message_list();
	create_message_list();

	send_buf->msg_len = msg_len;
	memset( send_buf->mtext, '0', msg_len );

	//for(i = 0; i < 20; i++)
	for(;;)
	{
		// recv from queue
		memset( recv_buf->mtext, 0, msg_len );
		recv_message( dst_task_id, recv_buf, sizeof(message_buf) + msg_len, MSG_WAIT );
		printf("recv len %d:\n%s\n", recv_buf->msg_len, recv_buf->mtext );

		//send msg back to client
		memset( send_buf->mtext, 0, msg_len );
		send_buf->msg_len = recv_buf->msg_len;
		memcpy( send_buf->mtext, recv_buf->mtext, recv_buf->msg_len );
		printf("send: \n%s\n", send_buf->mtext);
		send_message( src_task_id, dst_task_id, (message_buf*)send_buf );
	}

	free( send_buf );
	free( recv_buf );
	return 0;
}

