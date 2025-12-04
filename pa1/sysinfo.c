#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/statvfs.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>

#define CONFIG_FILENAME ".sysinfo.conf"

/* ---------- Time printing ---------- */

/* Print local time and system time (UTC). */
void print_times(void) {
    time_t now = time(NULL);
    if (now == (time_t)-1) {
        perror("time");
        return;
    }

    struct tm *lt = localtime(&now);
    struct tm *utc = gmtime(&now);

    char buf_local[64];
    char buf_utc[64];

    if (lt && strftime(buf_local, sizeof(buf_local), "%Y-%m-%d %H:%M:%S", lt)) {
        printf("Local time:          %s\n", buf_local);
    } else {
        fprintf(stderr, "Failed to get local time\n");
    }

    if (utc && strftime(buf_utc, sizeof(buf_utc), "%Y-%m-%d %H:%M:%S", utc)) {
        printf("System time (UTC):   %s\n", buf_utc);
    } else {
        fprintf(stderr, "Failed to get UTC time\n");
    }
}

/* ---------- Mount points ---------- */

/*
 * Read /proc/mounts and for each mount point call statvfs()
 * to show filesystem type, total space and free space.
 */
void print_mount_points(void) {
    FILE *f = fopen("/proc/mounts", "r");
    if (!f) {
        perror("fopen /proc/mounts");
        return;
    }

    printf("\nMount points and space:\n");
    printf("%-25s %-10s %18s %18s\n",
           "Mount point", "Type", "Total bytes", "Free bytes");

    char dev[256], mnt[256], type[64], opts[256];
    int dump, pass;

    while (fscanf(f, "%255s %255s %63s %255s %d %d\n",
                  dev, mnt, type, opts, &dump, &pass) == 6) {

        struct statvfs vfs;
        if (statvfs(mnt, &vfs) == 0) {
            unsigned long long total =
                (unsigned long long)vfs.f_blocks * vfs.f_frsize;
            unsigned long long free =
                (unsigned long long)vfs.f_bfree * vfs.f_frsize;
            printf("%-25s %-10s %18llu %18llu\n",
                   mnt, type, total, free);
        }
    }

    fclose(f);
}

/* ---------- Basic system info ---------- */

/* Print OS name/version and hostname. */
void print_system_info_header(void) {
    struct utsname u;
    char hostname[256];

    if (uname(&u) == 0) {
        printf("OS:       %s %s\n", u.sysname, u.release);
    } else {
        perror("uname");
    }

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    } else {
        perror("gethostname");
    }

    print_times();
    print_mount_points();
}

/* ---------- Directory listing ---------- */

/*
 * Convert st_mode to a permission string like "-rwxr-xr-x".
 */
void mode_to_permissions(mode_t mode, char perms[11]) {
    strcpy(perms, "----------");

    if (S_ISDIR(mode)) perms[0] = 'd';
    else if (S_ISLNK(mode)) perms[0] = 'l';
    else if (S_ISCHR(mode)) perms[0] = 'c';
    else if (S_ISBLK(mode)) perms[0] = 'b';
    else if (S_ISSOCK(mode)) perms[0] = 's';
    else if (S_ISFIFO(mode)) perms[0] = 'p';

    if (mode & S_IRUSR) perms[1] = 'r';
    if (mode & S_IWUSR) perms[2] = 'w';
    if (mode & S_IXUSR) perms[3] = 'x';

    if (mode & S_IRGRP) perms[4] = 'r';
    if (mode & S_IWGRP) perms[5] = 'w';
    if (mode & S_IXGRP) perms[6] = 'x';

    if (mode & S_IROTH) perms[7] = 'r';
    if (mode & S_IWOTH) perms[8] = 'w';
    if (mode & S_IXOTH) perms[9] = 'x';
}

/*
 * List all entries directly inside the given directory:
 * permissions, size, last modification time, and name.
 */
void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    printf("\nListing for: %s\n", path);
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[1024];
        struct stat st;
        char perms[11];
        char timebuf[64];

        /* Build full path: path + "/" + entry->d_name */
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        mode_to_permissions(st.st_mode, perms);

        struct tm *mt = localtime(&st.st_mtime);
        if (!mt || !strftime(timebuf, sizeof(timebuf),
                             "%Y-%m-%d %H:%M:%S", mt)) {
            strcpy(timebuf, "N/A");
        }

        printf("%s %10ld %s %s\n",
               perms,
               (long)st.st_size,
               timebuf,
               entry->d_name);
    }

    closedir(dir);
}

/* ---------- Settings: config file (~/.sysinfo.conf) ---------- */

/*
 * Build path to the config file: $HOME/.sysinfo.conf
 * Returns pointer to static buffer, or NULL on error.
 */
char *get_config_path(void) {
    const char *home = getenv("HOME");
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        if (pw) home = pw->pw_dir;
    }
    if (!home) return NULL;

    static char path[1024];
    snprintf(path, sizeof(path), "%s/%s", home, CONFIG_FILENAME);
    return path;
}

/* Save last viewed directory into config file. */
void save_last_dir(const char *dir) {
    char *conf = get_config_path();
    if (!conf) return;

    FILE *f = fopen(conf, "w");
    if (!f) {
        perror("fopen config");
        return;
    }
    fprintf(f, "last_dir=%s\n", dir);
    fclose(f);
}

/* Load last viewed directory from config file (if present). */
void load_last_dir(char *buf, size_t sz) {
    char *conf = get_config_path();
    if (!conf) return;

    FILE *f = fopen(conf, "r");
    if (!f) return;

    char line[1024];
    if (fgets(line, sizeof(line), f)) {
        /* very simple format: last_dir=/some/path */
        if (sscanf(line, "last_dir=%1023s", buf) == 1) {
            /* loaded successfully */
        }
    }
    fclose(f);
}

/* ---------- Interactive menu (extra feature) ---------- */

void interactive_menu(void) {
    char last_dir[1024] = {0};
    load_last_dir(last_dir, sizeof(last_dir));

    printf("\n=== Interactive System Info UI ===\n");
    if (last_dir[0]) {
        printf("Last viewed directory from config: %s\n", last_dir);
    }

    while (1) {
        printf("\nMenu:\n");
        printf("  1) Show system info & mount points\n");
        printf("  2) List a directory\n");
        printf("  3) Run a shell command (mini terminal)\n");
        printf("  4) Quit\n");
        printf("Choose an option: ");

        char line[256];
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\nEOF detected, exiting.\n");
            break;
        }
        int choice = atoi(line);

        if (choice == 1) {
            printf("\n--- System information ---\n");
            print_system_info_header();
        } else if (choice == 2) {
            char path[1024];
            printf("Enter directory path: ");
            if (!fgets(path, sizeof(path), stdin)) {
                break;
            }
            size_t len = strlen(path);
            if (len > 0 && path[len - 1] == '\n') path[len - 1] = '\0';
            if (path[0] == '\0') {
                printf("Empty path, skipping.\n");
                continue;
            }
            list_directory(path);
            save_last_dir(path);
            strncpy(last_dir, path, sizeof(last_dir) - 1);
            last_dir[sizeof(last_dir) - 1] = '\0';
        } else if (choice == 3) {
            char cmd[1024];
            printf("Enter command to run (empty to cancel): ");
            if (!fgets(cmd, sizeof(cmd), stdin)) {
                break;
            }
            size_t len = strlen(cmd);
            if (len > 0 && cmd[len - 1] == '\n') cmd[len - 1] = '\0';
            if (cmd[0] == '\0') {
                printf("No command entered.\n");
                continue;
            }
            printf("\n--- Running: %s ---\n", cmd);
            int status = system(cmd);
            printf("\nCommand finished with status: %d\n", status);
        } else if (choice == 4) {
            printf("Exiting interactive UI.\n");
            break;
        } else {
            printf("Invalid choice. Please enter 1–4.\n");
        }
    }
}

/* ---------- main ---------- */

int main(int argc, char *argv[]) {
    printf("=== sysinfo tool ===\n");
    print_system_info_header();

    char last_dir[1024] = {0};
    load_last_dir(last_dir, sizeof(last_dir));
    if (last_dir[0]) {
        printf("\nLast viewed directory from config: %s\n", last_dir);
    }

    if (argc > 1) {
        /* Assignment-required non-interactive mode */
        list_directory(argv[1]);
        save_last_dir(argv[1]);
    } else {
        /* Extra: interactive text UI */
        interactive_menu();
    }

    return 0;
}
