#!/usr/bin/python
# coding=utf-8

import socket
import threading
# 使用TCP流协议
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind(('0.0.0.0', 1234))
s.listen(5)


def tcpline(sock, addr):
    print('Accept new connection from %s:%s...' % addr)
    while True:
        data = sock.recv(4096)
        if 'exit' in data:
            break
        print(data)
    sock.close()
    print('close connection from %s:%s...' % addr)
    
while True:
    socket, addr = s.accept()
    t = threading.Thread(target=tcpline, args=(socket, addr))
    t.start()