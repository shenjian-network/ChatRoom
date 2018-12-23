#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    int ret, i, status;
    pid_t pid;
    while(1){
        pid = fork(); 
        if (pid == -1) {
            fprintf(stderr, "fork() error.errno:%d error:%s/n", errno, strerror(errno));
            break;
        }
        if (pid == 0) {
            ret = execv("server",NULL);
            //ret = execl(child_argv[0], "portmap", NULL, 0);
            if (ret < 0) {
                fprintf(stderr, "execv ret:%d errno:%d error:%s/n", ret, errno, strerror(errno));
                continue;
            }
            exit(0);
        }
        if (pid > 0) {
            pid = wait(&status);
            sleep(10);
            printf("return status:%d\n", status);
        }
    }
    return 0;
}
