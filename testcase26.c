#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

static char rot13_char(char c) {
    if (c >= 'a' && c <= 'z') {
        return ((c - 'a' + 13) % 26) + 'a';
    } else if (c >= 'A' && c <= 'Z') {
        return ((c - 'A' + 13) % 26) + 'A';
    }
    return c;
}

static void rot13_string(const char *input, char *output) {
    int i;
    for (i = 0; input[i] != '\0'; i++) {
        output[i] = rot13_char(input[i]);
    }
    output[i] = '\0'; 
}

typedef struct {
    char *root_dir;
} rot13_data;

#define ROT13_DATA ((rot13_data *) fuse_get_context()->private_data)

static void get_real_path(const char *path, char *real_path) {
    char decrypted_path[4096];
    rot13_string(path, decrypted_path);
    snprintf(real_path, 4096, "%s%s", ROT13_DATA->root_dir, decrypted_path);
}

static int rot13_getattr(const char *path, struct stat *stbuf,
                         struct fuse_file_info *fi) {
    (void) fi;
    int res;
    char real_path[4096];

    get_real_path(path, real_path);

    res = lstat(real_path, stbuf);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int rot13_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi,
                         enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    char real_path[4096];
    get_real_path(path, real_path);

    DIR *dp = opendir(real_path);
    if (dp == NULL) {
        return -errno;
    }

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char encrypted_name[256];
        rot13_string(de->d_name, encrypted_name);
        
        if (filler(buf, encrypted_name, &st, 0, 0)) {
            break;
        }
    }

    closedir(dp);
    return 0;
}

static int rot13_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char real_path[4096];

    get_real_path(path, real_path);

    res = open(real_path, fi->flags);
    if (res == -1) {
        return -errno;
    }

    fi->fh = res;
    return 0;
}

static int rot13_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    (void) path;
    int res = pread(fi->fh, buf, size, offset);
    if (res == -1) {
        res = -errno;
    }
    return res;
}

static int rot13_release(const char *path, struct fuse_file_info *fi) {
	(void) path;
	close(fi->fh);
	return 0;
}

static const struct fuse_operations rot13_oper = {
    .getattr = rot13_getattr,
    .readdir = rot13_readdir,
    .open    = rot13_open,
    .read    = rot13_read,
    .release = rot13_release,
};

int main(int argc, char *argv[]) {
    const char *src_dir = "rot13source";
    const char *mnt_dir = "mntrot13";

    mkdir(src_dir, 0755);
    mkdir(mnt_dir, 0755);


    rot13_data *data = malloc(sizeof(rot13_data));
    if (data == NULL) {
        perror("malloc");
        return 1;
    }
    
    data->root_dir = realpath(src_dir, NULL);
    if(data->root_dir == NULL) {
        perror("realpath");
        free(data);
        return 1;
    }

    char *fuse_argv[] = { argv[0], (char *)mnt_dir, NULL };
    int fuse_argc = 2;

    return fuse_main(fuse_argc, fuse_argv, &rot13_oper, data);
}