#ifndef FILE_API_H
#define FILE_API_H

#define FILE_READ_ENTIRE(name) void *name(const char *URL, size_t *FileSize)
typedef FILE_READ_ENTIRE(file_read_entire_fn);

struct file_api
{
    file_read_entire_fn *ReadEntire;
};

#endif // FILE_API_H