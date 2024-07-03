#include "enzyme.h"

int please_quit;
int use_yield;
int workperformed;

pthread_mutex_t lock;

void print_info_string(thread_info_t *info) {
    printf("%c%c\n", *(info->string[0]), *(info->string[1]));
}

// The code each enzyme executes.
void *run_enzyme(void *data) {
    // 1. Cast the void* pointer to thread_info_t*
    thread_info_t *thread_info = (thread_info_t *)data;
    // 2. Initialize the swapcount to zero
    thread_info->swapcount = 0;
    // 3. Set the cancel type to PTHREAD_CANCEL_ASYNCHRONOUS
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    // 4. If the first letter of the string is a C then call pthread_cancel on this thread
    if (*(thread_info->string[0]) == 'C') {
        pthread_cancel(pthread_self());
    }
    // 5. Create a while loop that only exits when please_quit is nonzero
    while (please_quit == 0) {
        // 6. If the first character of the string has an ascii value greater than the second (s[0] >s[1])
        if (*(thread_info->string[0]) > *(thread_info->string[1])) {
            pthread_mutex_lock(&lock);
            // Set workperformed=1
            workperformed = 1;
            // Increment swapcount for this thread
            thread_info->swapcount++;
            // Swap the two characters around
            char temp = *(thread_info->string[0]);
            *(thread_info->string[0]) = *(thread_info->string[1]);
            *(thread_info->string[1]) = temp;
            workperformed = 0;
            pthread_mutex_unlock(&lock);
        }
        // If use_yield is nonzero then call sched_yield at the end of the loop.
        if (use_yield != 0) {
            sched_yield();
        }
    }
    // 7. Return a pointer to the updated structure.
    return (void *)thread_info;
}

// Make threads to sort string.
// Returns the number of threads created.
int make_enzyme_threads(pthread_t *enzymes, char *string, void *(*fp)(void *)) {
    int i,rv,len;
    thread_info_t *info;
    len = strlen(string);

    for (i = 0; i < len-1; i++) {
        info = (thread_info_t *)malloc(sizeof(thread_info_t));
        for (int j = 0; j < 2; j++) {
            info->string[j] = &string[i+j];
        }
        rv = pthread_create(&enzymes[i], NULL, fp, (void *)info);
        if (rv != 0) {
            fprintf(stderr, "Can't create thread %d.\n", i);
            free(info);
            exit(1);
        }
    }
    return len-1;
}

// Join all threads at the end.
// Returns the total number of swaps.
int join_on_enzymes(pthread_t *threads, int n) {
    int i;
    int totalswapcount = 0;

    for (i = 0; i < n; i++) {
        void *status;
	int rv = pthread_join(threads[i],&status);

        if (rv != 0) {
            fprintf(stderr,"Can't join thread %d:%s.\n",i,strerror(rv));
            continue;
        }

        if (status == PTHREAD_CANCELED) {
            printf("Thread %d was canceled.\n",i);
            continue;
            //exit(1);
        }
        else if (status == NULL) {
            printf("Thread %d did not return anything\n",i);
        }
        else {
            printf("Thread %d exited normally: ",i);
            // Hint - you will need to cast something.
            thread_info_t *info = (thread_info_t *)status;
            int threadswapcount = info->swapcount;
            printf("%d swaps.\n",threadswapcount);
            totalswapcount += threadswapcount;
            free(info);
        }
    }	
    return totalswapcount;
}

/* Wait until the string is in order. Note, we need the workperformed flag just in case a thread is in the middle of swapping characters
so that the string temporarily is in order because the swap is not complete.
*/
void wait_till_done(char *string, int n) {
    int i;
    while (1) {
        if (workperformed) {
            continue;
        }
        int done = 1;
        for (i = 1; i <= n; i++) {
            if (string[i-1] > string[i]) {
                done = 0;
                break;
            }
        }
        if (done) {
            break;
        }
    }
}

void *sleeper_func(void *p) {
    sleep( (int) p); 
    // Actually this may return before p seconds because of signals. 
    // See man sleep for more information
    printf("sleeper func woke up - exiting the program\n");
    exit(1);
}

int main(int argc, char **argv) {
    pthread_t enzymes[MAX];
    int n,totalswap;
    char string[MAX];

    if (argc <= 1) {
        fprintf(stderr,"Usage: %s <word>\n",argv[0]);
        exit(1);
    }
    strncpy(string,argv[1],MAX);

    please_quit = 0;
    use_yield = 1;

    printf("Creating threads...\n");
    n = make_enzyme_threads(enzymes,string,run_enzyme);
    printf("Done creating %d threads.\n",n);

    pthread_t sleeperid;
    pthread_create(&sleeperid,NULL,sleeper_func,(void*)5);
    wait_till_done(string,n);
    please_quit = 1;
    printf("Joining threads...\n");
    totalswap = join_on_enzymes(enzymes, n);
    printf("Total: %d swaps\n",totalswap);
    printf("Sorted string: %s\n",string);

    exit(0);
}
