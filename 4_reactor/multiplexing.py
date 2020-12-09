import socket
import select

def main():

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('', 9000))
    server_socket.listen(5)
    poll = select.poll()
    poll.register(server_socket.fileno(), select.POLLIN)

    connections = {}

    while True:
        events = poll.poll(10000)
        print("len(events) :{}".format(len(events)))
        for fileno, event in events:
            if fileno == server_socket.fileno():
                (cli_socket , cli_addr) = server_socket.accept()
                poll.register(cli_socket.fileno(), select.POLLIN)
                connections[cli_socket.fileno()] = cli_socket
                
                print("connection fro: {}".format(cli_addr))
            elif event & select.POLLIN:
                cli_socket = connections[fileno]
                data = cli_socket.recv(4096)
                if data:
                    print("recv data from:{} data:{}".format(cli_socket, data))
                    for fn, sck in connections.items():
                        peername = cli_socket.getpeername()
                        print('peername: {}'.format(peername))
                        # sck.send(str(str(peername[0]) + " : " + str(peername[1])+ " >> " + str(data)).encode('utf8'))
                        sck.send(data)

                    # cli_socket.send(data)
                else:
                    print("close {}".format(cli_socket))
                    poll.unregister(fileno)
                    cli_socket.close()
                    del connections[fileno]
                    # connections.popitem()


    pass

if __name__ == "__main__":
    main()
    pass
