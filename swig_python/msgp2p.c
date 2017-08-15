// 
// test queue msg send & recv
// command line:  p2p my_id peer_id max_len
// input "bye" to break the loop
//

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "msg.h"

struct task_info {
	int dst_task_id;
	int src_task_id;
	int msg_len;
};

//
// message sending samples
//
void send_msg_thread( void *arg )
{
	MSG_BUF* send_buf;
	struct task_info *param;
	char* str;

	param = (struct task_info *)arg;

	// malloc msg block
	send_buf = MALLOC_MSG( param->msg_len );
	if( send_buf == NULL )
	{
		fprintf(stderr, "out of memory %d\n", __LINE__ );
		return ;
	}

	// start to send input message
	str = (char*)malloc(param->msg_len);
	for(;;)
	{
		printf("input send str: ");
		scanf("%s", str);

		// fix C language string lib bug, can not exceed max len
		str[param->msg_len] = 0;
		// copy str to message text
		FILL_MSG_BUF( send_buf, str, strlen(str) );

		// send to dst task
		send_message( param->dst_task_id, param->src_task_id, send_buf );

		if( strcmp( str, "bye") == 0 )
			break;
	}

	FREE_MSG( send_buf );
	free(str);
	return ;
}


//
// samples of msg handle
//
int msg_handle( MSG_BUF* msg )
{
	if( msg == NULL ) 
		return -1;

	printf("recv from task:%d, len:%d text:%s\n", GET_MSG_SRC_TID(msg), GET_MSG_LEN(msg), GET_MSG_TEXT(msg));

	if( strncmp( GET_MSG_TEXT(msg), "bye", 3 ) == 0 )
		return -1;
	else
		return 0;
}


//
// receive message thread
//
void recv_msg_thread( void *arg )
{
	MSG_BUF* recv_buf;
	struct task_info *param;
	char* str;
	int max_len;
	int ret;

	param = (struct task_info *)arg;

	// alloc the max size of msg buf
	max_len = MAX_MSG_BUF_SIZE; 		
	recv_buf = MALLOC_MSG( max_len );
	if( recv_buf == NULL )
	{
		fprintf(stderr, "out of memory %d\n", __LINE__ );
		return ;
	}

	for(;;)
	{
		// clear buffer
		memset( GET_MSG_TEXT(recv_buf), 0, max_len );

		// recv msg from queue
		if(( ret = recv_message( param->src_task_id, recv_buf, CALC_MSG_BUF_SIZE( max_len ), MSG_WAIT ) ) < 0 )
		{
			fprintf(stderr, "recv_message error\n");
			break;
		}

		//
		// here can be any msg handle
		//
		if( msg_handle(recv_buf) < 0 )
			break;

	}

	FREE_MSG( recv_buf );
	return;
}


// main entry
int main( int argc, char* argv[] )
{
	int dst_task_id;
	int src_task_id;
	int msg_len;
	pthread_t id_send_thread, id_recv_thread;
	struct task_info param;
	int ret;

	if( argc < 4 )
	{
		printf("usage: p2p my_id peer_id max_len \n");
		return -1;
	}

	src_task_id = atoi(argv[1]);
	dst_task_id = atoi(argv[2]);
	msg_len 	= atoi(argv[3]);
	printf("peer id:%d, my id: %d, max_len: %d\n", dst_task_id, src_task_id, msg_len );

	// create my msg queue
	create_message_queue( src_task_id );

	param.dst_task_id = dst_task_id;
	param.src_task_id = src_task_id;
	param.msg_len = msg_len;

    ret = pthread_create( &id_send_thread, NULL, (void  *)send_msg_thread, &param );  
    if(ret!=0)  
    {  
        printf("Create pthread error!\n");  
    	return -1;  
    } 

    ret = pthread_create( &id_recv_thread, NULL, (void  *)recv_msg_thread, &param );  
    if(ret!=0)  
    {  
        printf("Create pthread error!\n");  
    	return -1;  
    }     

    pthread_join(id_send_thread, NULL);  
    pthread_join(id_recv_thread, NULL);  

    // create my msg queue
	remove_message_queue( src_task_id );

	return 0;
}
