#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

void printHeader() {
    char hostname[HOST_NAME_MAX + 1];
    char *username = getenv("USER");

    if(username == NULL) {
        username = "user";
    }

    if(gethostname(hostname, sizeof(hostname)) == 0) {
        hostname[HOST_NAME_MAX] = '\0';
        printf("\033[1;32m%s\033[0m@\033[1;36m%s\033[0m\n", username, hostname);
        size_t len = strlen(username) + 1 + strlen(hostname);
        for (size_t i = 0; i < len; i++) {
            putchar('-');
        }
        putchar('\n');
    }
}

void getLinux_distro(char *buffer, size_t size) {
    FILE *fp = fopen("/etc/os-release", "r");
    if(fp == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    snprintf(buffer, size, "Unknown");

    char line[256];

    while(fgets(line, sizeof(line), fp)) {
        if(strncmp(line, "NAME=", 5) == 0) {
            char *start = line + 5;
            if(*start == '"') start++;
            char *end = strpbrk(start, "\"\n");
            if(end) *end = '\0';
            snprintf(buffer, size, "%s", start);
            break;
        }
    }
    fclose(fp);
}

void getKernel(char *buffer, size_t size) {
    struct utsname buf;

    if(uname(&buf) == 0) {
        snprintf(buffer, size, "%s %s", buf.sysname, buf.release);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getPackages(char *buffer, size_t size) {
    char line[256];
    if(access("/usr/bin/pacman", X_OK) == 0) {
        FILE *fp = popen("pacman -Qq | wc -l", "r");
        if(fp != NULL) {
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = '\0';
            pclose(fp);
            snprintf(buffer, size, "%s (pacman)", line);
        }
    } else if(access("/usr/bin/dpkg-query", X_OK) == 0) {
        FILE *fp = popen("dpkg-query -f '${binary:Package}\n' -W | wc -l", "r");
        if (fp != NULL) {
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = '\0';
            pclose(fp);
            snprintf(buffer, size, "%s (dpkg)", line);
        }
    } else if(access("/usr/bin/rpm", X_OK) == 0) {
        FILE *fp = popen("rpm -qa | wc -l", "r");
        if(fp != NULL) {
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = '\0';
            pclose(fp);
            snprintf(buffer, size, "%s (rpm)", line);
        }
    } else if(access("/sbin/apk", X_OK) == 0) {
        FILE *fp = popen("apk info | wc -l", "r");
        if (fp != NULL) {
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = '\0';
            pclose(fp);
            snprintf(buffer, size, "%s (apk)", line);
        }
    } else if(access("/usr/bin/qlist", X_OK) == 0) {
        FILE *fp = popen("qlist -I | wc -l", "r");
        if (fp != NULL) {
            fgets(line, sizeof(line), fp);
            line[strcspn(line, "\n")] = '\0';
            pclose(fp);
            snprintf(buffer, size, "%s (portage)", line);
        }
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getUptime(char *buffer, size_t size) {
    struct sysinfo info;

    if(sysinfo(&info) == 0) {
        snprintf(buffer, size, "%ld hours %ld minutes", info.uptime / 3600, (info.uptime % 3600) / 60);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getShell(char *buffer, size_t size) {
    char *shell_path = getenv("SHELL");

    if(shell_path == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    char *shell_name = strrchr(shell_path, '/');
    if(shell_name != NULL) {
        snprintf(buffer, size, "%s", shell_name + 1);
    } else {
        snprintf(buffer, size, "%s", shell_path);
    }
}

void getTerm(char *buffer, size_t size) {
    char *term_p_name = getenv("TERM_PROGRAM");
    char *term_name = getenv("TERM");

    if(term_p_name != NULL) {
        snprintf(buffer, size, "%s", term_p_name);
    } else if(term_name != NULL) {
        snprintf(buffer, size, "%s", term_name);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getDe_wm(char *buffer, size_t size) {
    char *xcd = getenv("XDG_CURRENT_DESKTOP");
    char *xsd = getenv("XDG_SESSION_DESKTOP");
    char *ds = getenv("DESKTOP_SESSION");

    if(xcd != NULL) {
        snprintf(buffer, size, "%s", xcd);
    } else if(xsd != NULL) {
        snprintf(buffer, size, "%s", xsd);
    } else if(ds != NULL) {
        snprintf(buffer, size, "%s", ds);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

#include <stdio.h>
#include <string.h>

void getMotherboard(char *buffer, size_t size) {
    snprintf(buffer, size, "Unknown");

    FILE *fp = fopen("/sys/class/dmi/id/board_name", "r");
    if (fp) {
        if (fgets(buffer, size, fp)) {
            buffer[strcspn(buffer, "\r\n")] = '\0';
        }
        fclose(fp);
    }
}

void getCpu(char *buffer, size_t size) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(fp == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    snprintf(buffer, size, "Unknown");

    char line[256];

    while(fgets(line, sizeof(line), fp)) {
        if(strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if(colon) {
                char *start = colon + 1;
                while (*start == ' ' || *start == '\t') start++;
                char *end = strchr(start, '\n');
                if (end) *end = '\0';
                snprintf(buffer, size, "%s", start);
                break;
            }
        }
    }
    fclose(fp);
}

void getGpu(char *buffer, size_t size) {
    char line[256];
    FILE *fp = popen("lspci | grep -E 'VGA|3D|Display'", "r");

    snprintf(buffer, size, "Unknown");

    if(fp != NULL && fgets(line, sizeof(line), fp) != NULL) {
        char *first_colon = strchr(line, ':');
        if(first_colon) {
            char *second_colon = strchr(first_colon + 1, ':');
            if(second_colon) {
                char *start = second_colon + 1;
                while (*start == ' ' || *start == '\t') start++;
                char *end = strchr(start, '\n');
                if (end) *end = '\0';
                snprintf(buffer, size, "%s", start);
            }
        }
    }
    if(fp != NULL) {
        pclose(fp);
    }
}

void getRam(char *buffer, size_t size) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if(fp == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    snprintf(buffer, size, "Unknown");

    unsigned long total = 0;
    unsigned long available = 0;
    char line[256];

    while(fgets(line, sizeof(line), fp) != NULL) {
        if(sscanf(line, "MemTotal: %lu", &total) == 1) {}
        if(sscanf(line, "MemAvailable: %lu", &available) == 1) {}
        if (total > 0 && available > 0) break;
    }
    fclose(fp);

    if (total > 0 && available > 0) {
        unsigned long used_mib = (total - available) / 1024;
        unsigned long total_mib = total / 1024;

        snprintf(buffer, size, "%luMiB / %luMiB", used_mib, total_mib);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getDisk(char *buffer, size_t size) {
    struct statvfs buf;

    if(statvfs("/", &buf) == 0) {
        snprintf(buffer, size, "%luGiB / %luGiB", ((buf.f_blocks - buf.f_bavail) * buf.f_frsize / (1024 * 1024 * 1024)), ((buf.f_blocks * buf.f_frsize) / (1024 * 1024 * 1024)));
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

#include <stdio.h>
#include <string.h>

void getSwap(char *buffer, size_t size) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    unsigned long total_kb = 0;
    unsigned long free_kb = 0;
    int found_total = 0, found_free = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "SwapTotal: %lu kB", &total_kb) == 1) {
            found_total = 1;
        } else if (sscanf(line, "SwapFree: %lu kB", &free_kb) == 1) {
            found_free = 1;
        }

        if (found_total && found_free) break;
    }
    fclose(fp);

    if (!found_total || total_kb == 0) {
        snprintf(buffer, size, "None");
        return;
    }

    unsigned long used_kb = (total_kb > free_kb) ? (total_kb - free_kb) : 0;

    unsigned long used_mib = used_kb / 1024;
    unsigned long total_mib = total_kb / 1024;

    snprintf(buffer, size, "%luMiB / %luMiB", used_mib, total_mib);
}
// next: void getIp()
