#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

void print_header() {
    char hostname[HOST_NAME_MAX + 1];
    char *username = getenv("USER");

    if (username == NULL) {
        username = "user";
    }

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        hostname[HOST_NAME_MAX] = '\0';
        printf("\033[1;32m%s\033[0m@\033[1;36m%s\033[0m\n", username, hostname);
        size_t len = strlen(username) + 1 + strlen(hostname);
        for (size_t i = 0; i < len; i++) {
            putchar('-');
        }
        putchar('\n');
    }
}

void get_linux_distro(char *distro, size_t size) {
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp == NULL) {
        snprintf(distro, size, "Unknown");
        return;
    }

    snprintf(distro, size, "Unknown");

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "NAME=", 5) == 0) {
            char *start = line + 5;
            if (*start == '"') start++;
            char *end = strpbrk(start, "\"\n");
            if (end) *end = '\0';
            snprintf(distro, size, "%s", start);
            break;
        }
    }
    fclose(fp);
}

void get_kernel(char *kernel_v, size_t size) {
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        snprintf(kernel_v, size, "%s %s", buffer.sysname, buffer.release);
    } else {
        snprintf(kernel_v, size, "Unknown");
    }
}

void get_uptime(char *uptime, size_t size) {
    struct sysinfo info;

    if(sysinfo(&info) == 0) {
        snprintf(uptime, size, "%ld hours %ld minutes", info.uptime / 3600, (info.uptime % 3600) / 60);
    } else {
        snprintf(uptime, size, "Unknown");
    }
}

void get_shell(char *shell, size_t size) {
    char *shell_path = getenv("SHELL");

    if(shell_path == NULL) {
        snprintf(shell, size, "Unknown");
        return;
    }

    char *shell_name = strrchr(shell_path, '/');
    if(shell_name != NULL) {
        snprintf(shell, size, "%s", shell_name + 1);
    } else {
        snprintf(shell, size, "%s", shell_path);
    }
}

void get_term(char *term, size_t size) {
    char *term_p_name = getenv("TERM_PROGRAM");
    char *term_name = getenv("TERM");

    if(term_p_name != NULL) {
        snprintf(term, size, "%s", term_p_name);
    } else if(term_name != NULL) {
        snprintf(term, size, "%s", term_name);
    } else {
        snprintf(term, size, "Unknown");
    }
}

void get_de_wm(char *dewm, size_t size) {
    char *xcd = getenv("XDG_CURRENT_DESKTOP");
    char *xsd = getenv("XDG_SESSION_DESKTOP");
    char *ds = getenv("DESKTOP_SESSION");

    if(xcd != NULL) {
        snprintf(dewm, size, "%s", xcd);
    } else if(xsd != NULL) {
        snprintf(dewm, size, "%s", xsd);
    } else if(ds != NULL) {
        snprintf(dewm, size, "%s", ds);
    } else {
        snprintf(dewm, size, "Unknown");
    }
}

void get_cpu(char *cpu, size_t size) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        snprintf(cpu, size, "Unknown");
        return;
    }

    snprintf(cpu, size, "Unknown");

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
                char *start = colon + 1;
                while (*start == ' ' || *start == '\t') start++;
                char *end = strchr(start, '\n');
                if (end) *end = '\0';
                snprintf(cpu, size, "%s", start);
                break;
            }
        }
    }
    fclose(fp);
}

int main() {
#ifdef __linux__
    print_header();

    char distro[128];
    get_linux_distro(distro, sizeof(distro));
    printf("\033[1;36mOS:\033[0m %s\n", distro);

    char kernel[256];
    get_kernel(kernel, sizeof(kernel));
    printf("\033[1;36mKernel:\033[0m %s\n", kernel);

    char uptime[128];
    get_uptime(uptime, sizeof(uptime));
    printf("\033[1;36mUptime:\033[0m %s\n", uptime);

    char shell[128];
    get_shell(shell, sizeof(shell));
    printf("\033[1;36mShell:\033[0m %s\n", shell);

    char term[128];
    get_term(term, sizeof(term));
    printf("\033[1;36mTerminal:\033[0m %s\n", term);

    char dewm[128];
    get_de_wm(dewm, sizeof(dewm));
    printf("\033[1;36mDE/WM:\033[0m %s\n", dewm);

    char cpu[128];
    get_cpu(cpu, sizeof(cpu));
    printf("\033[1;36mCPU:\033[0m %s\n", cpu);

#else
    printf("This program has only Linux support.\n");
#endif
    return 0;
}
