#ifndef GETINFO_H
#define GETINFO_H

#include <stddef.h>

void printHeader(void);
void getLinux_distro(char *buffer, size_t size);
void getKernel(char *buffer, size_t size);
void getPackages(char *buffer, size_t size);
void getUptime(char *buffer, size_t size);
void getShell(char *buffer, size_t size);
void getTerm(char *buffer, size_t size);
void getDe_wm(char *buffer, size_t size);
void getCpu(char *buffer, size_t size);
void getGpu(char *buffer, size_t size);
void getMotherboard(char *buffer, size_t size);
void getRam(char *buffer, size_t size);
void getDisk(char *buffer, size_t size);
void getSwap(char *buffer, size_t size);

#endif // GETINFO_H
