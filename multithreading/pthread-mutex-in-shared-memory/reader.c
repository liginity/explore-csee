#include "common.h"

int main() {
    int shm_fd;
    shared_data_t *shared_data;

    // Open existing shared memory
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Map shared memory
    shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Read data with mutex protection
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&shared_data->mutex);
        printf("Reader: Read '%s'\n", shared_data->message);
        fflush(stdout);

        sleep(2);
        pthread_mutex_unlock(&shared_data->mutex);

        sleep(1);
    }

    const int N = 1000 * 1000;
    for (int i = 0; i < N; ++i) {
        pthread_mutex_lock(&shared_data->mutex);
        ++shared_data->count;
        pthread_mutex_unlock(&shared_data->mutex);
    }
    printf("reader: count = %d\n", shared_data->count);

    // Cleanup
    munmap(shared_data, sizeof(shared_data_t));
    close(shm_fd);

    return 0;
}
