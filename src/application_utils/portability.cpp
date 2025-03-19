/// @file portability.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/types.h>

static_assert(sizeof(void *) == 8, "HIF requires to be compiled with 64-bit compiler");

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if (defined _MSC_VER)
#include <windows.h>
#pragma warning(push)
// disabling unreferenced params under Windows
#pragma warning(disable : 4100)
#elif (defined __MINGW32__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace application_utils
{

namespace /* anon */
{

// VC++ & MinGW
#if (defined _WIN32) // && ! (defined __GNUC__)
// This function is unix only. Re-implemented as a tmpfile for Windows.
FILE *_fmemopen(const char *buffer, int size, const char * /*mode*/, const char *path)
{
    // Seems to not work, since it creates the file into the root dir, which required administrative priviledges,
    // thus let's create manually a tmp file:
    //FILE * tmp = tmpfile();
    std::string dir(path);
    char *tmpname = _tempnam(dir.c_str(), "macro");
    if (tmpname == nullptr) {
        messageError("Unable to create a valid tmp file name.", nullptr, nullptr);
    }
#ifndef NDEBUG
    messageWarning((std::string("Creating tmp file: ") + tmpname).c_str(), nullptr, nullptr);
#endif
    FILE *tmp = fopen(tmpname, "w+");
    if (tmp == nullptr) {
        messageError("Unable to open tmp file to expand a macro (1).", nullptr, nullptr);
    }

    if (fprintf(tmp, "%s", buffer) != size) {
        messageError("Unable to initialize tmp file to expand a macro.", nullptr, nullptr);
    }

    // Closing and re-opening to reset internal flags and move seek to begin of file:
    fclose(tmp);
    tmp = fopen(tmpname, "rD");
    if (tmp == nullptr) {
        messageError("Unable to open tmp file to expand a macro (2).", nullptr, nullptr);
    }

    free(tmpname);
    return tmp;
}
#else
auto _fmemopen(const char *buffer, int size, const char *mode, const char * /*path*/) -> FILE *
{
    return fmemopen(const_cast<char *>(buffer), static_cast<size_t>(size), mode);
}
#endif

} // namespace

auto hif_strcasecmp(const char *s1, const char *s2) -> int
{
#if (defined _WIN32)
    return _stricmp(s1, s2);
#else
    return strcasecmp(s1, s2);
#endif
}

auto hif_strncasecmp(const char *s1, const char *s2, const size_t size) -> int
{
#if (defined _WIN32)
    return _strnicmp(s1, s2, size);
#else
    return strncasecmp(s1, s2, size);
#endif
}

auto hif_strtoll(const char *s1, char **s2, int base) -> long long
{
#if (defined _WIN32)
    return strtol(s1, s2, base);
#else
    return strtoll(s1, s2, base);
#endif
}

auto hif_getcwd(char *buf, size_t size) -> char *
{
#if (defined _WIN32)
    return _getcwd(buf, static_cast<int>(size));
#else
    return getcwd(buf, size);
#endif
}

auto hif_chmod(const char *path, int m) -> int
{
#if (defined _WIN32)
    return _chmod(path, m);
#else
    return chmod(path, static_cast<__mode_t>(m));
#endif
}

auto hif_chdir(const char *path) -> int
{
#if (defined _WIN32)
    return _chdir(path);
#else
    return chdir(path);
#endif
}

auto hif_rmdir(const char *path) -> int
{
#if (defined _WIN32)
    return _rmdir(path);
#else
    return rmdir(path);
#endif
}

auto hif_mkdir(const char *path, int mode) -> int
{
// Also for MinGW
#if (defined _WIN32)
    return _mkdir(path);
#else
    return mkdir(path, static_cast<__mode_t>(mode));
#endif
}

auto hif_strdup(const char *s) -> char *
{
#if (defined _WIN32)
    return _strdup(s);
#else
    return strdup(s);
#endif
}

auto hif_fileno(FILE *f) -> int
{
#if (defined _WIN32)
    return _fileno(f);
#else
    return fileno(f);
#endif
}

auto hif_isatty(int fd) -> int
{
#if (defined _WIN32)
    return _isatty(fd);
#else
    return isatty(fd);
#endif
}

auto hif_isdir(unsigned int mode) -> int
{
#if (defined _WIN32)
    return (mode & S_IFDIR);
#else
    return S_ISDIR(mode);
#endif
}

auto hif_islink(unsigned int mode) -> int
{
// Also for MinGW
#if defined(_WIN32)
    // Windows does not have links.
    return 0;
#else
    return S_ISLNK(mode);
#endif
}

auto hif_getfilesize(struct stat &s) -> int
{
// Also for MinGW
#if defined(_WIN32)
    return s.st_size;
#else
    return static_cast<int>(s.st_blocks);
#endif
}

auto hif_symlink(const char *s1, const char *s2) -> int
{
// Also for MinGW
#if defined(_WIN32)
    // Windows does not have links.
    return 0;
#else
    return symlink(s1, s2);
#endif
}

auto hif_round(const double d) -> double
{
// Also for MinGW
#if defined(_WIN32)
    return floor(d + 0.5);
#else
    return round(d);
#endif
}

auto hif_log2(const double d) -> double
{
// Also for MinGW
#if defined(_WIN32)
    return log(d) / log(2.0);
#else
    return log2(d);
#endif
}

auto hif_fmemopen(const char *buffer, int size, const char *mode, const char *path) -> FILE *
{
    return _fmemopen(buffer, size, mode, path);
}

auto hif_fdopen(const int fd, const char *const mode) -> FILE *
{
#if (defined _WIN32)
    return _fdopen(fd, mode);
#else
    return fdopen(fd, mode);
#endif
}

auto hif_getCurrentTimeAsString() -> std::string
{
#if (defined _MSC_VER)
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";
#if 0
    char result[100] = {0};
    //static DWORD first = GetTickCount();
    snprintf(result, 100, "%s"/*.%06ld*/, buffer/*, (long)(GetTickCount() - first)*/);

#endif

    std::string result;
    result = buffer;
    return result;
#else
    struct timeval tv {
    };
    gettimeofday(&tv, nullptr);
    char buffer[100];
#ifdef __MINGW32__
    time_t tt = tv.tv_sec;
    strftime(buffer, sizeof(buffer), "%X", localtime(&tt));
#else
    tm r{};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&tv.tv_sec, &r));
#endif
    char result[100];
    snprintf(result, 100, "%s" /*.%06ld"*/, buffer /*, (long)tv.tv_usec*/);
    return result;
#endif
}

auto hif_getCurrentDateAsString() -> std::string
{
    time_t rawtime      = 0;
    struct tm *timeinfo = nullptr;
    char buffer[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (strftime(buffer, 100, "%Y/%m/%d", timeinfo) == 0) {
        // error
        return "";
    }
    return buffer;
}

auto hif_getCurrentDateAndTimeAsString() -> std::string
{
    time_t rawtime      = 0;
    struct tm *timeinfo = nullptr;
    char buffer[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", timeinfo) == 0) {
        // error
        return "";
    }
    return buffer;
}

auto hif_getCurrentDateAndTimeAsFMIStringFormat() -> std::string
{
    time_t rawtime      = 0;
    struct tm *timeinfo = nullptr;
    char buffer[100];

    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    if (strftime(buffer, 100, "%Y-%m-%dT%H:%M:%S", timeinfo) == 0) {
        // error
        return "";
    }
    return buffer;
}

#if (defined _MSC_VER)
#pragma warning(pop)
#endif

// Constants.
// Also for MinGW
#if (defined _WIN32)
const int PERMISSION_RWX_USR = _S_IWRITE | _S_IREAD;
const int PERMISSION_R_USR   = _S_IREAD;
const int PERMISSION_W_USR   = _S_IWRITE;
const int PERMISSION_X_USR   = 0;
const int PERMISSION_RWX_GRP = _S_IWRITE | _S_IREAD;
const int PERMISSION_R_GRP   = _S_IREAD;
const int PERMISSION_W_GRP   = _S_IWRITE;
const int PERMISSION_X_GRP   = 0;
const int PERMISSION_RWX_OTH = _S_IWRITE | _S_IREAD;
const int PERMISSION_R_OTH   = _S_IREAD;
const int PERMISSION_W_OTH   = _S_IWRITE;
const int PERMISSION_X_OTH   = 0;
#else
const int PERMISSION_RWX_USR = S_IRWXU;
const int PERMISSION_R_USR   = S_IRUSR;
const int PERMISSION_W_USR   = S_IWUSR;
const int PERMISSION_X_USR   = S_IXUSR;
const int PERMISSION_RWX_GRP = S_IRWXG;
const int PERMISSION_R_GRP   = S_IRGRP;
const int PERMISSION_W_GRP   = S_IWGRP;
const int PERMISSION_X_GRP   = S_IXGRP;
const int PERMISSION_RWX_OTH = S_IRWXO;
const int PERMISSION_R_OTH   = S_IROTH;
const int PERMISSION_W_OTH   = S_IWOTH;
const int PERMISSION_X_OTH   = S_IXOTH;

#endif
} // namespace application_utils
} // namespace hif

/*
	// FILE SYSTEM SUPPORT
	#define symlink(x,y) 0
	#define S_ISLNK(X) 0 // windows don't have links
	#define S_ISDIR(x) x & S_IFDIR

    #define make_dir_hif_macro(x,y) _mkdir(x)

	#define FILE_SIZE st_size // field of stat construct
	#define DIR_PERM _S_IREAD | _S_IWRITE
	#define EXE_PERM _S_IREAD | _S_IWRITE
	// END FILE SYSTEM SUPPORT

    // Linux
	#include <dirent.h>
	#include <unistd.h>

	#define FILE_SIZE st_blocks // field of stat construct
	#define DIR_PERM S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
	#define EXE_PERM S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

	#define make_dir_hif_macro(x,y) mkdir(x,y)

#endif
#endif
	*/
