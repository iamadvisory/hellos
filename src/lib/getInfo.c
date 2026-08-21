#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <dirent.h>

void printHeader() {
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

void getLinux_distro(char *buffer, size_t size) {
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    snprintf(buffer, size, "Unknown");

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "NAME=", 5) == 0) {
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

    if (uname(&buf) == 0) {
        snprintf(buffer, size, "%s %s", buf.sysname, buf.release);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getPackages (char *buffer, size_t size) {
    DIR *dir = opendir("/var/lib/pacman/local");
    if (dir != NULL) {
        struct dirent *entry;
        int count = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            if (entry->d_type == DT_DIR) {
                count++;
            }
        }

        closedir(dir);
        snprintf(buffer, size, "%d (pacman)", count);
        return;
    }

    FILE *fp = fopen("/var/lib/dpkg/status", "r");
    if (fp != NULL) {
        int count = 0;
        char line[256];

        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "Status: install ok installed", 27) == 0) {
                count++;
            }
        }

        fclose(fp);
        snprintf(buffer, size, "%d (dpkg)", count);
        return;
    }

    fp = fopen("/var/lib/apk/installed", "r");
    if (fp != NULL) {
        int count = 0;
        char line[256];

        while (fgets(line, sizeof(line), fp) != NULL) {
            if(strncmp(line, "P:", 2) == 0) {
                count++;
            }
        }

        fclose(fp);
        snprintf(buffer, size, "%d (apk)", count);
        return;
    }

    dir = opendir("/var/db/pkg");
    if (dir != NULL) {
        struct dirent *entry;
        int count = 0;
        char cat_path[512];

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            if (entry->d_type == DT_DIR) {
                snprintf(cat_path, sizeof(cat_path), "/var/db/pkg/%s", entry->d_name);

                DIR *sub_dir = opendir(cat_path);
                if (sub_dir != NULL) {
                    struct dirent *sub_entry;

                    while ((sub_entry = readdir(sub_dir)) != NULL) {
                        if (strcmp(sub_entry->d_name, ".") == 0 || strcmp(sub_entry->d_name, "..") == 0) {
                            continue;
                        }
                        if (sub_entry->d_type == DT_DIR) {
                            count++;
                        }
                    }
                    closedir(sub_dir);
                }
            }
        }
        closedir(dir);
        snprintf(buffer, size, "%d (portage)", count);
        return;
    }
    snprintf(buffer, size, "Unknown");
}

void getUptime(char *buffer, size_t size) {
    struct sysinfo info;

    if (sysinfo(&info) == 0) {
        snprintf(buffer, size, "%ld hours %ld minutes", info.uptime / 3600, (info.uptime % 3600) / 60);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getShell(char *buffer, size_t size) {
    char *shell_path = getenv("SHELL");

    if (shell_path == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    char *shell_name = strrchr(shell_path, '/');
    if (shell_name != NULL) {
        snprintf(buffer, size, "%s", shell_name + 1);
    } else {
        snprintf(buffer, size, "%s", shell_path);
    }
}

void getTerm(char *buffer, size_t size) {
    char *term_p_name = getenv("TERM_PROGRAM");
    char *term_name = getenv("TERM");

    if (term_p_name != NULL) {
        snprintf(buffer, size, "%s", term_p_name);
    } else if (term_name != NULL) {
        snprintf(buffer, size, "%s", term_name);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

void getDe_wm(char *buffer, size_t size) {
    char *xcd = getenv("XDG_CURRENT_DESKTOP");
    char *xsd = getenv("XDG_SESSION_DESKTOP");
    char *ds = getenv("DESKTOP_SESSION");

    if (xcd != NULL) {
        snprintf(buffer, size, "%s", xcd);
    } else if (xsd != NULL) {
        snprintf(buffer, size, "%s", xsd);
    } else if (ds != NULL) {
        snprintf(buffer, size, "%s", ds);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

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
    if (fp == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    snprintf(buffer, size, "Unknown");

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
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
    snprintf(buffer, size, "Unknown");

    DIR *dir = opendir("/sys/bus/pci/devices");
    if (!dir) return;

    struct dirent *entry;
    unsigned int v_id = 0, d_id = 0;
    int found = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char path[512];
        snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/class", entry->d_name);
        FILE *f_class = fopen(path, "r");
        if (!f_class) continue;

        unsigned int class_code = 0;
        fscanf(f_class, "0x%x", &class_code);
        fclose(f_class);

        if ((class_code >> 16) == 0x03) {
            snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/vendor", entry->d_name);
            FILE *f_v = fopen(path, "r");
            snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/device", entry->d_name);
            FILE *f_d = fopen(path, "r");

            if (f_v && f_d) {
                if (fscanf(f_v, "0x%x", &v_id) == 1 && fscanf(f_d, "0x%x", &d_id) == 1) {
                    found = 1;
                }
            }
            if (f_v) fclose(f_v);
            if (f_d) fclose(f_d);

            if (found) break;
        }
    }
    closedir(dir);

    if (!found) return;

    FILE *f_ids = fopen("/usr/share/hwdata/pci.ids", "r");
    if (!f_ids) {
        f_ids = fopen("/usr/share/misc/pci.ids", "r");
    }

    if (!f_ids) {
        snprintf(buffer, size, "PCI ID %04x:%04x", v_id, d_id);
        return;
    }

    char target_v[5], target_d[5];
    snprintf(target_v, sizeof(target_v), "%04x", v_id);
    snprintf(target_d, sizeof(target_d), "%04x", d_id);

    char line[512];
    char vendor_name[128] = "";
    char device_name[128] = "";
    int in_target_vendor = 0;

    while (fgets(line, sizeof(line), f_ids)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (line[0] != '\t') {
            if (in_target_vendor) break;

            if (strncasecmp(line, target_v, 4) == 0 && (line[4] == ' ' || line[4] == '\t')) {
                in_target_vendor = 1;
                char *name = line + 4;
                while (*name == ' ' || *name == '\t') name++;
                char *end = strpbrk(name, "\r\n");
                if (end) *end = '\0';
                strncpy(vendor_name, name, sizeof(vendor_name) - 1);
            }
        } else if (in_target_vendor && line[0] == '\t' && line[1] != '\t') {
            if (strncasecmp(line + 1, target_d, 4) == 0 && (line[5] == ' ' || line[5] == '\t')) {
                char *name = line + 5;
                while (*name == ' ' || *name == '\t') name++;
                char *end = strpbrk(name, "\r\n");
                if (end) *end = '\0';
                strncpy(device_name, name, sizeof(device_name) - 1);
                break;
            }
        }
    }
    fclose(f_ids);

    if (in_target_vendor && device_name[0] != '\0') {
        snprintf(buffer, size, "%s %s", vendor_name, device_name);
    } else if (in_target_vendor) {
        snprintf(buffer, size, "%s [%04x]", vendor_name, d_id);
    } else {
        snprintf(buffer, size, "PCI ID %04x:%04x", v_id, d_id);
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

    if (statvfs("/", &buf) == 0) {
        unsigned long long used =
            (unsigned long long)(buf.f_blocks - buf.f_bavail) *
            (unsigned long long)buf.f_frsize /
            (1024ULL * 1024ULL * 1024ULL);

        unsigned long long total =
            (unsigned long long)buf.f_blocks *
            (unsigned long long)buf.f_frsize /
            (1024ULL * 1024ULL * 1024ULL);

        snprintf(buffer, size, "%lluGiB / %lluGiB", used, total);
    } else {
        snprintf(buffer, size, "Unknown");
    }
}

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
