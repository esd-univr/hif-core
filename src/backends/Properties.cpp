/// @file Properties.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "hif/application_utils/FileStructure.hpp"
#include "hif/backends/Properties.hpp"

namespace hif
{
namespace backends
{

#define BUFSIZE 65536

Properties::Properties()
    : name()
    , props()
{
}

Properties::~Properties() {}

void Properties::load(const std::string &config_file_name, const char silent, const char evaluate)
{
    name = eval(config_file_name);
    std::ifstream stream(name.c_str());
    if (stream.fail()) {
        if (!silent) {
            printf("Cannot load config file %s.\n", eval(config_file_name).c_str());
        }
        return;
    }

    char *buf = new char[BUFSIZE];
    char *s1;
    char *s2;
    char *s3;

    do {
        stream.getline(buf, BUFSIZE);

        s1 = &(buf[strspn(buf, " \t")]);

        // skip comments or empty lines
        if (s1[0] == '#')
            continue;

        s3 = s2 = &(s1[strcspn(s1, " \t=")]);

        s2 = &(s2[strspn(s2, " \t")]);

        if (s2[0] != '=')
            continue;

        s2 = &(s2[1]);
        s2 = &(s2[strspn(s2, " \t")]);

        if (s2[0] == '\0')
            continue;

        s3[0] = '\0';

        if (evaluate)
            setProperty(s1, s2);
        else
            setProperty(s1, s2, 0);
    } while (!stream.eof());
    delete[] buf;
}

std::string Properties::operator[](const std::string &property_name) const { return getProperty(property_name); }

void Properties::dump(std::ostream &o) const
{
    for (std::map<std::string, std::string, std::less<std::string>>::const_iterator i = props.begin(); i != props.end();
         i++) {
        o << (*i).first << " = " << (*i).second << std::endl;
    }
}

void Properties::setProperty(const std::string &property_name, const std::string &property, const char evaluate)
{
    if (evaluate)
        props[property_name] = eval(property);
    else
        props[property_name] = property;
}

//
// Specific use
void Properties::appendProperty(const std::string &property_name, const std::string &property, const char evaluate)
{
    setProperty(property_name, property, evaluate);
    Properties iprops;
    iprops.load(name, 1, evaluate);
    iprops.setProperty(property_name, property, evaluate);
    std::ofstream oFile;
    hif::application_utils::FileStructure fFile(name);
    if (!(fFile.getParentFile().exists())) {
        fFile.getParentFile().make_dirs();
    }
    oFile.open(name.c_str());
    if (oFile.is_open()) {
        iprops.dump(oFile);
    }
}

std::string Properties::eval(const std::string &s_in) const
{
    int l;
    std::string s = hif::application_utils::FileStructure::eval(s_in);

    l = static_cast<int>(s.find("$("));
    if (l == -1) {
        return s;
    } else {
        std::string r(s, 0, static_cast<unsigned long>(l));
        std::string s1(s, static_cast<unsigned long>(l + 2));
        std::string evald_s1(eval(s1));
        int vl;

        vl = static_cast<int>(evald_s1.find(")"));
        if (vl != -1) {
            std::string varname(evald_s1, 0, static_cast<unsigned long>(vl));
            std::string r2(evald_s1, static_cast<unsigned long>(vl + 1));

            if (props.find(varname) != props.end()) {
                Properties *This = const_cast<Properties *>(this);
                // mutable not supported by SUN 5.0
                r += This->props[varname];
            } else {
                char *env_val = getenv(varname.c_str());

                if (env_val != nullptr) {
                    r += env_val;
                } else {
                    r += "$(" + varname + ")";
                }
            }
            r += r2;
        } else {
            r += "$(" + evald_s1;
        }

        return r;
    }
}

} // namespace backends
} // namespace hif
