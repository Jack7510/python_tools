#!/usr/bin/env python

import os
import msg
import signal
import sys
import time

def signal_handler(signal, frame):
    print 'You pressed Ctrl+C!'
    sys.exit(0)

# test program
if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    print 'Press Ctrl+C'

    msg.create_message_queue(1)
    msg.create_message_queue(0)

    buf = msg.byteArray(100)
    cmd = msg.intArray(1)
    src_id = msg.intArray(1)
    dst_id = 1

    while True:	
	msg_len = msg.recv_simple_message(dst_id, src_id, cmd, buf, 100)
        print 'from task ', src_id[0], 'cmd ', cmd[0], 'len ', msg_len
        msg.send_simple_message(src_id[0], dst_id, cmd[0], buf, msg_len)

        for i in range(0, msg_len):
            print buf[i] ,
        print '\n'
