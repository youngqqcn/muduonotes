
#todo: 使用locust进行测试
# from locust import HttpUser, task, between

import socket
from socket import AF_INET, socket, SOCK_STREAM, SHUT_RD
# import threading
from threading import Thread


def hello_to_server():

    sock = socket(family=AF_INET, type=SOCK_STREAM)
    ret = sock.connect_ex(address=("127.0.0.1", 9000))
    if ret == 0:
        print("connected succeed")
    else:
        print("connected failed")

    # while True:
    for n in range(10000):
        msg = "hello"
        ret = sock.send(data=msg.encode('utf8'))
        if ret < 0:
            print("send failed")
        else:
            print("send {} bytes successs".format(ret))
        recvbuf = sock.recv(1024)
        print(
            f'received  {len(recvbuf)} bytes from server, content is : {recvbuf.decode("utf8")}')

    sock.shutdown(SHUT_RD)
    sock.close()
    print("close sockfd")

    pass


def main():

    threads = []
    for n in range(100):
        thd = Thread(target=hello_to_server)
        threads.append(thd)
        thd.start()
        pass

    for thd in threads:
        thd.join()
    pass


if __name__ == "__main__":
    main()
    pass
