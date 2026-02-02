#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
int main()
{
    sleep(7);
    printf("Content-Type: html\r\n");
    printf("Content-Length: 29\r\n");
    printf("\r\n");
    printf("hwllo im walid khiar welcome");
}