/*
 * File		: msg.c
 * Desc		: implementation of message communication interfaces
 * Author	: Jack Lee
 * Date		: Feb 10, 2017
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "msg.h"
#include "msg.inc"

/*
 * Func: create_message_list
 * Desc: create all the message queues in system
 * Para: none
 * Ret : 0 if OK
 */
int create_message_queue_list(void)
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    int i;		/* for loop */

    /* create all msg queues */
    for( i = TID_LAUNCHER_HWD; i < TID_MAX; i++ )
    {
    	key = TASK_TO_MQKEY(i);
	    if ((msqid = msgget(key, msgflg )) < 0) {
	        perror("msgget");
	        return -1;
	    }
	    else 
     		(void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);
    }

	return 0;
}


/*
 * Func: remove_message_list
 * Desc: remove all the message queues in system
 * Para: none
 * Ret : 0 if OK
 */
int remove_message_queue_list(void)
{
    int msqid;
    int msgflg = 0666;
    key_t key;
    int i;		/* for loop */

    /* create all msg queues */
    for( i = TID_LAUNCHER_HWD; i < TID_MAX; i++ )
    {
    	key = TASK_TO_MQKEY(i);
	    if ((msqid = msgget(key, msgflg )) < 0) {
	        perror("msgget");
	        //return -1;
	    }
	    else 
	    {
     		/* Remove the queue */
        	msgctl(msqid, IPC_RMID, 0);
     	}
    }

	return 0;
}


/*
 * Func: create_message
 * Desc: create a message queue for task id
 * Para: task_id
 * Ret : 0 if OK
 */
int create_message_queue( int task_id )
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;

    /* create all msg queues */
    if( (task_id >= TID_LAUNCHER_HWD) && (task_id < TID_MAX) )
    {
    	key = TASK_TO_MQKEY(task_id);
	    if ((msqid = msgget(key, msgflg)) < 0) {
	        perror("msgget");
	        return -1;
	    }
	    
     	(void) fprintf(stderr,"msgget: msgget succeeded: taskid = %d, msqid = %d\n", task_id, msqid);
     	return 0;
    }

	return -1;
}


/*
 * Func: remove_message_queue
 * Desc: remove the message queue of task id in system
 * Para: none
 * Ret : 0 if OK
 */
int remove_message_queue( int task_id )
{
    int msqid;
    int msgflg = 0666;
    key_t key;

    /* create all msg queues */
    if( task_id >= TID_LAUNCHER_HWD && task_id < TID_MAX )
    {
    	key = TASK_TO_MQKEY(task_id);
	    if ((msqid = msgget(key, msgflg )) < 0) {
	        perror("msgget");
	        //return -1;
	    }
	    else 
	    {
     		/* Remove the queue */
        	msgctl(msqid, IPC_RMID, 0);
     	}
    }

	return 0;
}


/*
 * Func: send_message
 * Desc: send message to dst task 
 * Para: dst_task_id - task id of recv task id
 *		 src_task_id - task id of sender task id
 *		 msg         - message to be sent
 *		 msg_len	 - the length of msg
 * Ret : 0 if OK
 */
int send_message( int dst_task_id, int src_task_id, MSG_BUF* msg )
{
	int msqid;
    key_t key;
 
 	if( !CHECK_TASK_ID(dst_task_id) 
 		|| !CHECK_TASK_ID(src_task_id)
 		|| (msg == NULL)
 		|| (GET_MSG_LEN(msg) > MAX_MSG_BUF_SIZE)	/* can not exceed MAX_MSG_BUF_SIZE */
 	  ) 
 	{
 		perror("send_message param error");
 		return -1;
 	}

    key = TASK_TO_MQKEY(dst_task_id);;
    if ((msqid = msgget(key, 0666)) < 0) 
    {
        fprintf(stderr, "msgget %s %d\n", __func__, __LINE__);
        return -1;
    }	

	/*
     * fill message buffer
     */
    msg->mtype 			= 1;
    msg->dst_task_id 	= dst_task_id;
    msg->src_task_id 	= src_task_id;
 
    /*
     * Send a message.
     */
    if (msgsnd(msqid, msg, MSG_LENGTH(msg), IPC_NOWAIT) < 0) 
    {
    	fprintf (stderr, "%d, %d\n", msqid, GET_MSG_LEN(msg));
        return -1;
    }

    return 0;
}


/*
 * Func: recv_message
 * Desc: receive message  
 * Para: task_id - task id of message to read
 *		 msg         - message to be sent
 *		 msg_len	 - the length of msg
 *		 msgflg		 - MSG_WAIT if wait, otherwise,
 * Ret : < 0 if fail, 0 is OK
 */
int recv_message( int task_id, MSG_BUF* msg, int msg_len, int msgflg )
{
    int msqid;
    key_t key;
    //MSG_BUF* qbuf;
    long len;
    int flag = (msgflg == MSG_WAIT) ? 0 : IPC_NOWAIT;

 	if( !CHECK_TASK_ID(task_id) || (msg == NULL)) 
 	{
 		perror("send_message param error");
 		return -1;
 	}

    key = TASK_TO_MQKEY(task_id);;
    if ((msqid = msgget(key, 0666)) < 0) 
    {
        fprintf(stderr, "msgget %s %d\n", __func__, __LINE__);
        return -1;
    }	

    /*
     * Receive an answer of message type 1.
     */
    if ( (len = msgrcv(msqid, msg, msg_len, 1, flag )) < 0) 
    {
        perror("msgrcv");
        return -1;
    }

    //fprintf(stderr, "recv_message %lu %d\n", MSG_LENGTH(msg), len);
    // receive a whole message
    if( MSG_LENGTH(msg) == len && (msg->dst_task_id == task_id) )
    	return 0;
    else
    {
    	fprintf(stderr, "recv_message: len or task id failed %ld %d\n", len, task_id );
		return -1;
	}
}


/*
 * Func: send_simple_message
 * Desc: send message to dst task 
 * Para: dst_task_id - task id of recv task id
 *       src_task_id - task id of sender task id
 *       msg         - message to be sent
 *       msg_len     - the length of msg
 *       command     - command of parameter
 * Ret : 0 if OK
 */
int send_simple_message( int dst_task_id, int src_task_id, int command, char* msg, int msg_len )
{
    int msqid;
    key_t key;
    MSG_BUF* msg_buf;

    if( !CHECK_TASK_ID(dst_task_id) 
        || !CHECK_TASK_ID(src_task_id)
      ) 
    {
        perror("send_message param error");
        return -1;
    }

    key = TASK_TO_MQKEY(dst_task_id);;
    if ((msqid = msgget(key, 0666)) < 0) 
    {
        fprintf(stderr, "msgget %s %d %d\n", __func__, __LINE__, dst_task_id);
        return -1;
    }   

    msg_buf = (MSG_BUF*)MALLOC_MSG(msg_len);
    if( msg_buf == NULL ) 
    {
        printf("%s %d malloc fail\n", __func__, __LINE__ );
        return -1;
    }

    bzero(msg_buf, msg_len);

    /*
     * fill message buffer
     */
    msg_buf->mtype          = 1;
    msg_buf->dst_task_id    = dst_task_id;
    msg_buf->src_task_id    = src_task_id;
    msg_buf->command        = command;
    msg_buf->msg_len        = msg_len;
    if( msg_len )
        memcpy( msg_buf->mtext, msg, msg_len );
 
    /*
     * Send a message.
     */
    if (msgsnd(msqid, msg_buf, MSG_LENGTH(msg_buf), IPC_NOWAIT) < 0) 
    {
        fprintf (stderr, "%d, %d\n", msqid, GET_MSG_LEN(msg_buf));
        FREE_MSG(msg_buf);
        return -1;
    }

    FREE_MSG(msg_buf);
    return 0;
}


/*
 * Func: recv_message
 * Desc: receive message  
 * Para: task_id - task id of message to read
 *       msg         - message to be sent
 *       msg_len     - the length of msg
 *       msgflg      - MSG_WAIT if wait, otherwise,
 * Ret : < 0 if fail, 0 is OK
 */
int recv_simple_message( int task_id, int* src_task_id, int* cmd, char* msg, int msg_len )
{
    int msqid;
    key_t key;
    MSG_BUF* recv_buf;
    long len;
    //int flag = (msgflg == MSG_WAIT) ? 0 : IPC_NOWAIT;
    int flag =  0 ;

    //printf(" task_id %d, src_task %d, cmd %d, len %d\n", task_id, *src_task_id, *cmd, msg_len );

    if( !CHECK_TASK_ID(task_id) || (msg == NULL)) 
    {
        perror("send_message param error");
        return -1;
    }

    key = TASK_TO_MQKEY(task_id);;
    if ((msqid = msgget(key, 0666)) < 0) 
    {
        fprintf(stderr, "msgget %s %d\n", __func__, __LINE__);
        return -1;
    }   

    recv_buf = (MSG_BUF*) malloc( sizeof(MSG_BUF) + msg_len );
    if( recv_buf == NULL ) 
    {
        perror("malloc fail!\n");
        return -1;
    }

    /*
     * Receive an answer of message type 1.
     */
    if ( (len = msgrcv(msqid, recv_buf, msg_len, 1, flag )) < 0) 
    {
        free(recv_buf);
        perror("msgrcv");
        return -1;
    }

    //fprintf(stderr, "recv_message %lu %d\n", MSG_LENGTH(msg), len);
    // receive a whole message
    if( MSG_LENGTH(recv_buf) == len && (recv_buf->dst_task_id == task_id) )
    {
        *cmd = recv_buf->command;
        memcpy( msg, recv_buf->mtext, msg_len);
        len = recv_buf->msg_len;
        //printf("src task %d, cmd %d, len %ld\n", recv_buf->src_task_id, *cmd, len );
        free(recv_buf);
        return len;
    }
    else
    {
        free(recv_buf);
        fprintf(stderr, "recv_message: len or task id failed %ld %d\n", len, task_id );
        return -1;
    }
}


