#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

#define SHM_NAME "/my_shared_memory"

typedef struct {
    pthread_mutex_t mutex;
    char message[256];
} shared_data_t;

#endif
