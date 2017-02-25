#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;
struct thread_arg{
    bool thread_ended = 0;
    pthread_mutex_t* mutext;
};

static void * func2_thread(void *arg){
    thread_arg* parsed_arg = (thread_arg*) arg;
    while(!(parsed_arg->thread_ended)){
        pthread_mutex_lock(parsed_arg->mutext);
        for(int i = 0; i < 3; ++i){
            cout << "2";
            fflush(stdout);
            sleep(1);
        }
        pthread_mutex_unlock(parsed_arg->mutext);
        sleep(1);
    }
    pthread_exit((void*) "\nThread 2 ended");
}

static void * func1_thread(void *arg){
    thread_arg* parsed_arg = (thread_arg*) arg;
    while (!(parsed_arg->thread_ended)){
        while (!(parsed_arg->thread_ended) && !(pthread_mutex_trylock(parsed_arg->mutext) ) ){
            for(int i = 0; i < 3; ++i){
                cout << "1";
                fflush(stdout);
                sleep(1);
            }
            pthread_mutex_unlock(parsed_arg->mutext);
            sleep(2);
        }
        cout << "\nThread 1 waiting unlock, and do something else\n";
        sleep(2);
    }
    pthread_exit((void*) "\nThread 1 ended");
}

int main(int argc, char *argv[])
{
    pthread_t thread1, thread2;
    thread_arg thread1_arg, thread2_arg;
    char* exit_thread_code1 = "";
    char* exit_thread_code2 = "";

    pthread_mutex_t my_super_mutex;
    if(pthread_mutex_init(&my_super_mutex, NULL))
        cerr << "Cant init mutex";
    thread1_arg.mutext = &my_super_mutex;
    thread2_arg.mutext = &my_super_mutex;

    if ( pthread_create( &thread1, NULL, func1_thread, &thread1_arg ) )
            return 1;
    if ( pthread_create( &thread2, NULL, func2_thread, &thread1_arg ) )
            return 1;

    getchar();
    thread1_arg.thread_ended = 1;
    thread2_arg.thread_ended = 1;

    if ( pthread_join( thread1, (void**) &exit_thread_code1 ) )
            return 1;
    cout<< exit_thread_code1 << endl;
    if ( pthread_join( thread2, (void**) &exit_thread_code2 ) )
            return 1;
    cout<< exit_thread_code2 << endl;

    return 0;
}
