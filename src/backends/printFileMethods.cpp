/// @file printFileMethods.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/backends/printFileMethods.hpp"

void hif::backends::splitFileName(const std::string &f, std::string &base, std::string &ext)
{
    std::string::size_type p = f.find_last_of('.');
    if (p == std::string::npos) {
        base = f;
        ext  = "";
        return;
    }

    base = f.substr(0, p);
    ext  = f.substr(p + 1);
}

int hif::backends::openFileStream(const std::string &name, std::ofstream *stream)
{
    stream->open(name.c_str());
    if (!stream->is_open() || !stream->good())
        return 0;
    return 1;
}

void hif::backends::closeFileStream(std::ofstream *stream) { stream->close(); }
