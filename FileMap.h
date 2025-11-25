#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char* data;
    size_t size;
} FileMap;

#ifdef _WIN32
#include <windows.h>

static int mapFile(const char* path, FileMap* out) {
    HANDLE hFile = CreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Erro ao abrir arquivo (%lu)\n", GetLastError());
        return 0;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        fprintf(stderr, "Erro GetFileSizeEx (%lu)\n", GetLastError());
        CloseHandle(hFile);
        return 0;
    }

    HANDLE hMap = CreateFileMappingA(
        hFile,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL
    );

    if (hMap == NULL) {
        fprintf(stderr, "Erro CreateFileMapping (%lu)\n", GetLastError());
        CloseHandle(hFile);
        return 0;
    }

    void* mapView = MapViewOfFile(
        hMap,
        FILE_MAP_READ,
        0,
        0,
        0
    );

    if (mapView == NULL) {
        fprintf(stderr, "Erro MapViewOfFile (%lu)\n", GetLastError());
        CloseHandle(hMap);
        CloseHandle(hFile);
        return 0;
    }

    out->data = (char*)mapView;
    out->size = (size_t)fileSize.QuadPart;

    CloseHandle(hMap);
    CloseHandle(hFile);
    return 1;
}

static void unmapFile(FileMap* fm) {
    if (fm->data) {
        UnmapViewOfFile(fm->data);
    }
    fm->data = NULL;
    fm->size = 0;
}

#else
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

static int mapFile(const char* path, FileMap* out) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 0;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return 0;
    }

    if (st.st_size == 0) {
        fprintf(stderr, "Arquivo vazio.\n");
        close(fd);
        return 0;
    }

    void* mapped = mmap(
        NULL,
        st.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );

    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 0;
    }

    close(fd);

    out->data = (char*)mapped;
    out->size = st.st_size;

    return 1;
}

static void unmapFile(FileMap* fm) {
    if (fm->data) {
        munmap(fm->data, fm->size);
    }
    fm->data = NULL;
    fm->size = 0;
}

#endif
