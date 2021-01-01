#include <sys/uio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char const *argv[])
{
    const char *pszTxt1 = "hellllllllll";
    const char *pszTxt2 = "hellllllllll";
    printf("============================\n");

    struct iovec iov[2];
    iov[0].iov_base = (void *)pszTxt1;
    iov[0].iov_len = strlen(pszTxt1) + 1;
    iov[1].iov_base = (void *)pszTxt2;
    iov[1].iov_len =strlen(pszTxt2) + 1;

    int nwrite = writev(STDOUT_FILENO, iov, 2);
    if(nwrite < 0)
    {
        printf("write error\n");
        return 0;
    }
    else
    {
        // printf("wroten %d bytes\n", nwrite);
    }
    

    // printf("writev succeed\n");
    return 0;
}
