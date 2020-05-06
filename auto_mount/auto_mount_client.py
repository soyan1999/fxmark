#!/usr/bin/env python3
import socket
import sys

class AutoMountClient:
    def __init__(self, port, cmd, mount_point):
        self.port = port
        self.cmd = cmd
        self.mount_point = mount_point
        self.client = None

    def run(self):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect(('127.0.0.1', self.port))
        if self.cmd == 'stop':
            self.client.send('stop'.encode('utf-8'))
        else:
            self.client.send((self.cmd + ' ' + self.mount_point).encode('utf-8'))
        ret = self.client.recv(4096).decode('utf-8')
        self.client.close()
        if ret == 'success':
            sys.exit(0)
        else:
            sys.exit(-1)       

if __name__ == '__main__':
    if len(sys.argv) == 3:
        client = AutoMountClient(int(sys.argv[1]), sys.argv[2], '')
        client.run()
    elif len(sys.argv) == 4:
        client = AutoMountClient(int(sys.argv[1]), sys.argv[2], sys.argv[3])
        client.run()
    else:
        print('nead two or three args')
        sys.exit(-1)
