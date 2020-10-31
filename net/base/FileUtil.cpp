//
// Created by 袁志红 on 2020/10/18.
//

#include "FileUtil.h"


AppendFile::AppendFile(const std::string &filename)
        : fp_(fopen(filename.c_str(), "ae")) {
    setbuffer(fp_, buf_, sizeof(buf_));
}

AppendFile::~AppendFile() {
    fclose(fp_);
}

void AppendFile::fluse() {
    fflush(fp_);
}

void AppendFile::append(const char *logline, const size_t len) {
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = this->write(logline + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) fprintf(stderr, "AppendFile::append() failed !\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

size_t AppendFile::write(const char *logline, size_t len) {
    return fwrite_unlocked(logline, 1, len, fp_);
}

