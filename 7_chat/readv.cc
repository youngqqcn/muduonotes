#include <sys/uio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char const *argv[])
{
    
    char szBuf1[5] = {0};
    char szBuf2[5] = {0};

    struct iovec iov[2];
    iov[0].iov_base = (void *)szBuf1;
    iov[0].iov_len = sizeof(szBuf1);
    iov[1].iov_base = (void *)szBuf2;
    iov[1].iov_len = sizeof(szBuf2);


    // 一次性从读完, 如果buf1满了 则向buf2中写
    int nread = readv(STDIN_FILENO, iov, sizeof(iov)/sizeof(iov[0]));
    if(nread < 0)
    {
        perror("write error\n");
        return 0;
    }

    printf("buf1 : %s\n", szBuf1);
    printf("buf2 : %s\n", szBuf2);

    return 0;
}
