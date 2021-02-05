#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_OF_THREADS 50

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct task_t {
    char text[100];
    int seconds;
    char filename[30];
    int is_working;
    int id;
    int to_close;
};

int get_number_of_working_threads(struct task_t tasks[]);

void display_working_threads(struct task_t tasks[], int threads_counter);

void *writer(void *arg) {
    struct task_t *info = arg;
    info->is_working = 1;

    while (info->to_close != 1) {
        sleep(info->seconds);

        pthread_mutex_lock(&mutex);
        FILE *file = fopen(info->filename, "a");
        fprintf(file, "%s\n", info->text);
        fclose(file);
        pthread_mutex_unlock(&mutex);
    }

    info->is_working = 0;

    return NULL;
}

void *manager(void *arg) {
    pthread_t thread_worker[NUM_OF_THREADS];

    int user_chosen_option = 0;
    int threads_counter = 0;

    struct task_t worker_task[NUM_OF_THREADS];

    while (user_chosen_option != -1) {
        printf("1 - Start new thread\n2 - Number of working threads\n3 - Stop a working thread\n4 - Number of stopped threads\n-1 - Exit\n\nEnter option: ");
        fflush(stdout);
        scanf("%d", &user_chosen_option);

        if (user_chosen_option == 1) {
            //TODO: need to handle bad filename or bad seconds entered by user
            printf("Enter filename: ");
            scanf("%s", worker_task[threads_counter].filename);
            printf("Enter text: ");
            scanf("%s", worker_task[threads_counter].text);
            printf("Enter seconds: ");
            scanf("%d", &worker_task[threads_counter].seconds);

            //TODO: should probably use stopped threads
            pthread_create(&thread_worker[threads_counter], NULL, writer, (void *) &worker_task[threads_counter]);
            worker_task[threads_counter].id = threads_counter;

            printf("\nCreated thread with ID: %d\n\n", threads_counter);

            threads_counter++;

        } else if (user_chosen_option == 2) {
            printf("\nNumber of working threads: %d\n", get_number_of_working_threads(worker_task));
            display_working_threads(worker_task, threads_counter);
        } else if (user_chosen_option == 3) {
            printf("\nWorking threads:\n");
            display_working_threads(worker_task, threads_counter);

            int id;

            printf("Enter ID: ");
            scanf("%d", &id);

            if (worker_task[id].is_working != 1 || worker_task[id].to_close == 1) {
                printf("\nYou've entered bad ID.\n\n");
            } else {
                worker_task[id].to_close = 1;
                printf("\nThread with ID %d stopped working.\n\n", id);
            }
        } else if (user_chosen_option == 4) {
            int how_many_stopped_threads = 0;
            for (int i = 0; i < threads_counter; i++) {
                if (worker_task[i].is_working == 0) {
                    how_many_stopped_threads++;
                }
            }
            printf("\nNumber of stopped threads: %d\n\n", how_many_stopped_threads);
            for (int i = 0; i < threads_counter; i++) {
                if (worker_task[i].is_working == 0) {
                    printf("[Thread ID: %d] Has been stopped.\n", worker_task[i].id);
                }
            }
            printf("\n");
        } else if (user_chosen_option == -1) {
            break;
        } else {
            printf("\nYou've entered bad number!\n\n");
        }
    }

    for (int i = 0; i < threads_counter; i++) {
        pthread_join(thread_worker[i], NULL);
    }

    return NULL;
}

int main(void) {

    pthread_t manager_thread;

    pthread_create(&manager_thread, NULL, manager, NULL);

    pthread_join(manager_thread, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}

int get_number_of_working_threads(struct task_t tasks[]) {
    if (tasks == NULL) {
        return 0;
    }

    int number_of_working_threads = 0;

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (tasks[i].is_working == 1) {
            number_of_working_threads++;
        }
    }

    return number_of_working_threads;
}

void display_working_threads(struct task_t tasks[], int threads_counter) {
    if (tasks != NULL) {
        for (int i = 0; i < threads_counter; i++) {
            if (tasks[i].is_working == 1) {
                printf("[Thread ID: %d] Writes \"%s\" every %d seconds.\n", tasks[i].id, tasks[i].text,
                       tasks[i].seconds);
            }
        }
        printf("\n");
    }
}