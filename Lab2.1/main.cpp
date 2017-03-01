#include <iostream>
#include <semaphore.h>
#include <string>
#include <unistd.h>
#include <time.h>
#include <errno.h>

using namespace std;
struct thread_arg{
    bool thread_ended;
    unsigned thread_num;
    sem_t* semaphore;
};

static void * thread_func(void *arg){
    thread_arg* parsed_arg = (thread_arg*) arg;
    struct timespec thread_waiting_time;

    while (!(parsed_arg->thread_ended)){
        //Initialise current time
        if (clock_gettime(CLOCK_REALTIME, &thread_waiting_time) == -1)
            cerr << "Can't set current time in clock_gettime() function";
        //Thread will be waiting 1 seconds
        thread_waiting_time.tv_sec += 1;

        int timedwait_status = sem_timedwait(parsed_arg->semaphore, &thread_waiting_time);
        int tmp_errno = errno; // Cause "errno" may be overwritten after next instruction
        if (!timedwait_status) {
            //Critical site of a code
            for(int i = 0; i < 3; ++i){
                cout << parsed_arg->thread_num;
                fflush(stdout);
            }
            sleep(1);
            sem_post(parsed_arg->semaphore);
        }
        else {
            if (tmp_errno == ETIMEDOUT){
                cout << "\nThread " << parsed_arg->thread_num
                     << " have timeout and do something else\n";
                //Something else
            }
        }
    }
    string tmp_end_str = "\nThread " + to_string(parsed_arg->thread_num) + " ended";
    pthread_exit((void*) tmp_end_str.c_str());
}

//Counting start with 0
#define amount_of_threads 3

int main(void)
{
    pthread_t  threads[amount_of_threads];
    thread_arg threads_args[amount_of_threads];

    sem_t    noname_sem;
    if(sem_init(&noname_sem, 0, 2))
        cerr << "Can't init semaphore";

    for (int i = 0; i < amount_of_threads; ++i){
        threads_args[i].thread_ended = 0;
        threads_args[i].thread_num   = i;
        threads_args[i].semaphore    = &noname_sem;
        if ( pthread_create( &threads[i], NULL, thread_func, &threads_args[i] ) )
                return 1;
    }

    getchar();

    //Close all threads after press ENTER
    for (int i = 0; i < amount_of_threads; ++i){
        threads_args[i].thread_ended = 1;
    }

    //Waiting while all threads ended
    for (int i = 0; i < amount_of_threads; ++i){
        char* exit_thread_code;
        if ( pthread_join( threads[i], (void**) &exit_thread_code ) )
                return 1;
        cout<< exit_thread_code << endl;
    }
    sem_destroy(&noname_sem);
    return 0;
}
