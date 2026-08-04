#include <stdio.h>
#include <stdarg.h>
#include "lib/getInfo.h"

#define COLOR_CYAN  "\033[1;36m"
#define COLOR_RESET "\033[0m"

void printInfo(const char *label, const char *fmt, ...) {
    printf(COLOR_CYAN "%s:" COLOR_RESET " ", label);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

int main(void) {
#ifdef __linux__
    printHeader();
    char buffer[256];

    getLinux_distro(buffer, sizeof(buffer));
    printInfo("OS", "%s", buffer);

    getKernel(buffer, sizeof(buffer));
    printInfo("Kernel", "%s", buffer);

    getPackages(buffer, sizeof(buffer));
    printInfo("Packages", "%s", buffer);

    getUptime(buffer, sizeof(buffer));
    printInfo("Uptime", "%s", buffer);

    getShell(buffer, sizeof(buffer));
    printInfo("Shell", "%s", buffer);

    getTerm(buffer, sizeof(buffer));
    printInfo("Terminal", "%s", buffer);

    getDe_wm(buffer, sizeof(buffer));
    printInfo("DE/WM", "%s", buffer);

    getCpu(buffer, sizeof(buffer));
    printInfo("CPU", "%s", buffer);

    getGpu(buffer, sizeof(buffer));
    printInfo("GPU", "%s", buffer);

    getMotherboard(buffer, sizeof(buffer));
    printInfo("Motherboard", "%s", buffer);

    getRam(buffer, sizeof(buffer));
    printInfo("RAM", "%s", buffer);

    getDisk(buffer, sizeof(buffer));
    printInfo("Disk (/)", "%s", buffer);

    getSwap(buffer, sizeof(buffer));
    printInfo("Swap", "%s", buffer);

    // next: getIp()

#else
    printf("This program has only Linux support.\n");
#endif
    return 0;
}
