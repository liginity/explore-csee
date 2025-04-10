#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#define PG_SIZE 4096

// round up to page boundary
#define ROUND_UP(addr) \
    (((unsigned long long)(addr) + PG_SIZE - 1) & ~(PG_SIZE - 1))

const char *shm_name = "/mmap_shm_at_addr";

// end of text segment, initialized data segment, uninitialized data segment
extern char etext;
extern char edata;
extern char end;

void mmap_shm_at_addr(void *addr, unsigned len);
void cleanup_shm();
int test_mmap_above_brk();
void error_exit(const char *error_info);

int test_mmap_above_brk() {
    char *brk0 = (void *)syscall(SYS_brk, NULL);
    printf("just call printf() first\n");
    printf("    there is memory allocation in printf()\n");

    // syscall brk
    char *brk1 = (void *)syscall(SYS_brk, NULL);
    // unsigned long long brk1 = syscall(SYS_brk, 0);
    assert(brk1 == sbrk(0));

    printf("&etext     = %p\n", &etext);
    printf("&edata     = %p\n", &edata);
    printf("&end       = %p\n", &end);
    printf("\n");

    printf("brk0       = %p\n", brk0);
    printf("brk1       = %p\n", brk1);
    if (brk0 != brk1) {
        // there is memory allocation in printf()
        printf("NOTE brk is changed after calling printf()\n");
    }

    char *addr1 = (char *)ROUND_UP(brk1);
    // addr1 = (char *)ROUND_UP(brk0);
    printf("addr1      = %p\n", addr1);

    unsigned shm_len = PG_SIZE * 1;
    mmap_shm_at_addr(addr1, shm_len);

    char *brk2 = (void *)syscall(SYS_brk, 0);
    printf("brk2       = %p\n", brk2);
    if (brk1 != brk2) {
        printf("brk is changed after calling mmap()\n");
    }

    int is_failed = 0;
    unsigned brk_inc_len2 = PG_SIZE * 4;

    char *sbrk_ret = sbrk(brk_inc_len2);
    char *brk3 = sbrk(0);
    if (sbrk_ret == (void *)-1 || brk3 <= brk1) {
        is_failed = 1;
    }

    // char *brk3 = (void *)syscall(SYS_brk, brk2 + brk_inc_len2);
    // if (brk3 <= brk2) {
    //     is_failed = 1;
    // }

    if (is_failed) {
        const char *error_info = strerror(errno);
        printf("error for last library call: %s\n", error_info);
        printf("failed to increase brk after using mmap()\n");
    } else {
        printf("succeeded to increase brk after using mmap()\n");
    }

    printf("brk3       = %p\n", brk3);

    cleanup_shm();

    return 0;
}

void mmap_shm_at_addr(void *addr, unsigned len) {
    // NOTE MAP_FIXED is not portable
    int fd;
    char *mmap_addr;
    // NOTE if the shared memory has small size,
    //      brk() call could succeed in my test.

    fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        error_exit("shm_open");
    }

    if (ftruncate(fd, len) == -1) {
        error_exit("ftruncate");
    }

    mmap_addr =
        mmap(addr, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd, 0);
    if (mmap_addr == MAP_FAILED) {
        error_exit("mmap");
    }

    // could close fd after calling mmap().
    close(fd);

    printf("mmap_addr  = %p\n", mmap_addr);
}

void cleanup_shm() { shm_unlink(shm_name); }

int main() {
    //
    test_mmap_above_brk();
    printf("finish test_mmap_above_brk()\n");
}

void error_exit(const char *error_info) {
    perror(error_info);
    fprintf(stderr, "exit on error\n");
    exit(EXIT_FAILURE);
}
