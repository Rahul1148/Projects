/*
 * syscall.c
 *
 *  Created on: May 6, 2025
 *      Author: Rahul
 */

#include "syscall.h"
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// Dummy implementations to satisfy the linker

int _close(int file) {
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file; (void)ptr; (void)dir;
    return 0;
}

int _read(int file, char *ptr, int len) {
    (void)file; (void)ptr; (void)len;
    errno = ENOSYS;
    return -1;
}


