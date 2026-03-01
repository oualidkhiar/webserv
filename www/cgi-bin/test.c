#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
int main()
{
    printf("Content-Type: html\r\n");
    printf("content-length: 29\r\n");
    printf("\r\n");
    printf("hwllo im walid khiar welcome");
}