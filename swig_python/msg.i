/* msg.i */

%module msg
%include "cpointer.i"
%include "carrays.i"
%include "cstring.i"
%include "cdata.i"

%array_class( char, byteArray );
%array_class(int, intArray);

int create_message_queue( int task_id );
int remove_message_queue( int task_id );
int send_simple_message( int dst_task_id, int src_task_id, int command, char * msg, int msg_len );	
int recv_simple_message( int task_id, int* src_task_id, int* command, char * msg, int msg_len );

enum TASK_ID
{
	TID_LAUNCHER_HWD = 0,		/* launcher Qt window */
	TID_CALL_HWD,				/* Voice call Qt window */
	TID_CALL_SERVICE,			/* Call service process */
	TID_ALARM_HWD,				/* Alarm Qt window */
	TID_ALARM_SERVCIE,			/* Alarm service process */
	TID_IM_HWD,					/* IM Qt window */
	TID_IM_SERVICE,				/* IM service process */
	TID_MAX
};

%{


int create_message_queue( int task_id );
int remove_message_queue( int task_id );
int send_simple_message( int dst_task_id, int src_task_id, int command, char* msg, int msg_len );	
int recv_simple_message( int task_id, int* src_task_id, int* cmd, char* msg, int msg_len );

enum TASK_ID
{
	TID_LAUNCHER_HWD = 0,		/* launcher Qt window */
	TID_CALL_HWD,				/* Voice call Qt window */
	TID_CALL_SERVICE,			/* Call service process */
	TID_ALARM_HWD,				/* Alarm Qt window */
	TID_ALARM_SERVCIE,			/* Alarm service process */
	TID_IM_HWD,					/* IM Qt window */
	TID_IM_SERVICE,				/* IM service process */
	TID_MAX
};

%}