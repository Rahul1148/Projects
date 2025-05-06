/*
 * syscall.h
 *
 *  Created on: May 6, 2025
 *      Author: Rahul
 */

#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <sys/stat.h>
#include <unistd.h>

int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _write(int file, char *ptr, int len);
int _read(int file, char *ptr, int len);

#endif /* SYSCALL_H_ */
