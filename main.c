#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>



//worker num
int num_child = 3;
//run status
int run = 1;

void sigctl(int sig) {
    run = 0;
}

int main(void) {
    signal(SIGTERM, sigctl);
    signal(SIGINT, sigctl);
    
    if (num_child > 0) {
        int child = 0;
        //monitor and run
        while (!child && run) {
            if (num_child > 0) {
                switch (fork()) {
                    case -1:
                        return -1;
                    case 0:    //child
                        child = 1;
                        break;
                    default:   //parent
                        num_child--;
                        break;
                }
            } else {
                int status;
                if (-1 != wait(&status)) {
                    num_child++;
                } else {
                    switch (errno) {
                        case EINTR:
                            fprintf(stderr, "%s\n", "eintr");
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        //recive a signal, run is 0
        if (!child) {
           fprintf(stderr, "%s-%d\n", "monitor", getpid()); 
           kill(0, SIGTERM);
           while(num_child--)
               wait();
           //sleep();
        }
    }
    //worker 
    if (run) {
        fprintf(stderr, "%s-%d\n", "worker", getpid()); 
        while(1) {
            sleep(1);
            fprintf(stderr, "%s:%d\n", "balabala", getpid());
            if (!run) break;
        }
    }
    fprintf(stderr, "%s-%d\n", "exit", getpid());

    return 0;
}

