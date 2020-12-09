#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <array>
#include <unordered_map>

typedef int EventType;

class Epoll {
    // 封装了epoll I/O 多路复用的机制, Event demultiplexer
public:
    static const int NO_FLAGS = 0;
    static const int BLOCK_INDEFINITELY = -1;
    static const int MAX_EVENTS = 5;

    Epoll() {
        fileDescriptor = epoll_create1(NO_FLAGS);
        event.data.fd = STDIN_FILENO;
        // 设置epoll event 为EPOLLIN(对应文件描述符可读)， EPOLLPRI(对应文件描述符有紧急事件可读)
        event.events = EPOLLIN | EPOLLPRI;
    }

    int wait() {
        return epoll_wait(fileDescriptor, events.data(), MAX_EVENTS, BLOCK_INDEFINITELY);
    }

    int control() {
        return epoll_ctl(fileDescriptor, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    }

    ~Epoll() {
        close(fileDescriptor);
    }

private:
    int fileDescriptor;
    struct epoll_event
            event;
    std::array<epoll_event, MAX_EVENTS> events{};

};

class EventHandler {
    // Event Handler

public:
    int handle_event(EventType et) {
        std::cout << "Event Handler: " << et << std::endl;
        return 0;
    }

};

class Reactor {
    // Dispatcher
public:
    Reactor() {
        epoll.control();
    }

    //注册对应的回调函数到handlers中
    void addHandler(std::string event, EventHandler callback) {
        handlers.emplace(std::move(event), std::move(callback));
    }

    void run() {
        while (true) {
            int numberOfEvents = wait();

            for (int i = 0; i < numberOfEvents; ++i) {
                std::string input;
                std::getline(std::cin, input);

                try {
                    // 根据的具体的事件去找对应的handler，并执行相应的操作
                    handlers.at(input).handle_event(EventType(i));
                } catch (const std::out_of_range &e) {
                    std::cout << "no  handler for " << input << std::endl;
                }
            }
        }
    }


private:
    // handlers Table, 存储事件以及其对应的handlers
    std::unordered_map<std::string, EventHandler> handlers{};
    Epoll epoll;

    int wait() {
        int numberOfEvents = epoll.wait();
        return numberOfEvents;
    }
};

int main() {
    Reactor reactor;

    reactor.addHandler("a", EventHandler{});
    reactor.addHandler("b", EventHandler{});
    reactor.run();
}