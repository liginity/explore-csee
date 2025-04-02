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

    // Write data with mutex protection
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&shared_data->mutex);
        snprintf(shared_data->message, sizeof(shared_data->message), "Message %d from writer", i);
        printf("Writer: Wrote '%s'\n", shared_data->message);
        pthread_mutex_unlock(&shared_data->mutex);
        sleep(1);
    }

    // Cleanup
    pthread_mutex_destroy(&shared_data->mutex);
    munmap(shared_data, sizeof(shared_data_t));
    close(shm_fd);
    shm_unlink(SHM_NAME);  // Remove shared memory after execution

    return 0;
}
