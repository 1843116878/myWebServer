//
// Created by 袁志红 on 2020/10/18.
//

#include <string>

#ifndef MYWEBSERVER_FILEUTIL_H
#define MYWEBSERVER_FILEUTIL_H

class AppendFile {
public:
    AppendFile(const AppendFile &) = delete;

    const AppendFile &operator=(const AppendFile &) = delete;

public:
    explicit AppendFile(const std::string &filename);

    ~AppendFile();

    void append(const char *logline, const size_t len);

    void fluse();

private:
    size_t write(const char *logline, size_t len);

    FILE *fp_;
    char buf_[64 * 1024];
};
#endif //MYWEBSERVER_FILEUTIL_H
