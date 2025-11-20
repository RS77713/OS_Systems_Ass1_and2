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

#define CONFIG_FILENAME ".sysinfo.conf"

void print_system_info() {
    struct utsname u;
    char hostname[256];

    if (uname(&u) == 0) {
        printf("OS: %s %s\n", u.sysname, u.release);
    } else {
        perror("uname");
    }

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    } else {
        perror("gethostname");
    }

    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    char buf[64];
    if (lt && strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)) {
        printf("Local time: %s\n", buf);
    }

    struct statvfs vfs;
    if (statvfs("/", &vfs) == 0) {
        unsigned long long total = vfs.f_blocks * vfs.f_frsize;
        unsigned long long free  = vfs.f_bfree  * vfs.f_frsize;
        printf("Disk (/): total=%llu bytes, free=%llu bytes\n", total, free);
    } else {
        perror("statvfs");
    }
}

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
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        char perms[11] = "----------";
        if (S_ISDIR(st.st_mode)) perms[0] = 'd';
        if (st.st_mode & S_IRUSR) perms[1] = 'r';
        if (st.st_mode & S_IWUSR) perms[2] = 'w';
        if (st.st_mode & S_IXUSR) perms[3] = 'x';
        if (st.st_mode & S_IRGRP) perms[4] = 'r';
        if (st.st_mode & S_IWGRP) perms[5] = 'w';
        if (st.st_mode & S_IXGRP) perms[6] = 'x';
        if (st.st_mode & S_IROTH) perms[7] = 'r';
        if (st.st_mode & S_IWOTH) perms[8] = 'w';
        if (st.st_mode & S_IXOTH) perms[9] = 'x';

        char timebuf[64];
        struct tm *mt = localtime(&st.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", mt);

        printf("%s %10ld %s %s\n",
               perms,
               (long)st.st_size,
               timebuf,
               entry->d_name);
    }

    closedir(dir);
}

char *get_config_path() {
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

void load_last_dir(char *buf, size_t sz) {
    char *conf = get_config_path();
    if (!conf) return;
    FILE *f = fopen(conf, "r");
    if (!f) return;

    char line[1024];
    if (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "last_dir=%1023s", buf) == 1) {
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    print_system_info();

    char last_dir[1024] = {0};
    load_last_dir(last_dir, sizeof(last_dir));
    if (last_dir[0]) {
        printf("Last viewed directory from config: %s\n", last_dir);
    }

    if (argc > 1) {
        list_directory(argv[1]);
        save_last_dir(argv[1]);
    } else {
        printf("Usage: %s <directory>\n", argv[0]);
    }

    return 0;
}
