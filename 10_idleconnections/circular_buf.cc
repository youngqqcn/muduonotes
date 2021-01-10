#include <boost/circular_buffer.hpp>
#include <iostream>


int main(int argc, char const *argv[])
{

    boost::circular_buffer<int> buf;
    // buf.resize(3);
    buf.set_capacity(3);

    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);
    buf.push_back(4); // 会覆盖第一个

    std::cout << "size=" << buf.size() <<  "cap = " << buf.capacity() << std::endl;

    for(auto it = buf.begin(); it != buf.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
    
    return 0;
}
