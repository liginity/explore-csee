#include "common.h"

int main() {
    int shm_fd;
    shared_data_t *shared_data;

    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Set size of shared memory
    ftruncate(shm_fd, sizeof(shared_data_t));

    // Map shared memory
    shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize mutex attributes
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    // Initialize mutex in shared memory
    pthread_mutex_init(&shared_data->mutex, &attr);
    shared_data->start_time = time(NULL);
    // Write data with mutex protection
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&shared_data->mutex);
        snprintf(shared_data->message, sizeof(shared_data->message), "Message %d from writer", i);
        int time_now = time(NULL) - shared_data->start_time;
        printf("(%d s) ", time_now);
        printf("Writer: Wrote '%s'\n", shared_data->message);
        fflush(stdout);

        sleep(2);
        pthread_mutex_unlock(&shared_data->mutex);

        sleep(1);
    }

    const int N = 1000 * 1000;
    for (int i = 0; i < N; ++i) {
        if (i == N * 3 / 4) {
            printf("writer (in loop): count = %d\n", shared_data->count);
            sleep(1);
        }
        pthread_mutex_lock(&shared_data->mutex);
        ++shared_data->count;
        pthread_mutex_unlock(&shared_data->mutex);
    }
    printf("writer: count = %d\n", shared_data->count);

    // Cleanup
    pthread_mutex_destroy(&shared_data->mutex);
    munmap(shared_data, sizeof(shared_data_t));
    close(shm_fd);
    shm_unlink(SHM_NAME);  // Remove shared memory after execution

    return 0;
}
