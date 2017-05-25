
#pragma once

#include <PL/platform_log.h>
#include <PL/platform_window.h>
#include <PL/platform_graphics.h>
#include <PL/platform_model.h>
#include <PL/platform_filesystem.h>

#define TITLE               "Piggy Viewer"
#define LOG                 "hog_loader"

#define PRINT(...)          printf(__VA_ARGS__); plWriteLog(LOG, __VA_ARGS__)
#define PRINT_ERROR(...)    PRINT(__VA_ARGS__); exit(-1)
#ifdef _DEBUG
#   define DPRINT(...)      PRINT(__VA_ARGS__)
#else
#   define DPRINT(...)      (__VA_ARGS__)
#endif

typedef struct GlobalVars {
    bool is_psx;

    unsigned int width, height;
} GlobalVars;

extern GlobalVars g_state;
