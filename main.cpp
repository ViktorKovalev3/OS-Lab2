#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>

using namespace std;
struct thread_arg{
    bool thread_ended = 0;
    unsigned thread_num = 0;
    pthread_mutex_t* mutext;
};

static void * thread_func(void *arg){
    thread_arg* parsed_arg = (thread_arg*) arg;
    while (!(parsed_arg->thread_ended)){
        while (!(parsed_arg->thread_ended) && !(pthread_mutex_trylock(parsed_arg->mutext) ) ){
            for(int i = 0; i < 3; ++i){
                cout << parsed_arg->thread_num;
                fflush(stdout);
                sleep(1);
            }
            pthread_mutex_unlock(parsed_arg->mutext);
            sleep(2);
        }
        cout << "\nThread" << parsed_arg->thread_num << "waiting unlock, and do something else\n";
        sleep(2);
    }
    string tmp_end_str = "\nThread" + to_string(parsed_arg->thread_num) + "ended";
    pthread_exit((void*) tmp_end_str.c_str());
}

//Counting start with 0
#define amount_of_threads 2

int main(void)
{
    pthread_t  threads[amount_of_threads];
    thread_arg threads_args[amount_of_threads];

    pthread_mutex_t my_super_mutex;
    if(pthread_mutex_init(&my_super_mutex, NULL))
        cerr << "Cant init mutex";

    for (int i = 0; i < amount_of_threads; ++i){
        threads_args[i].mutext      = &my_super_mutex;
        threads_args[i].thread_num  = i;
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

    return 0;
}