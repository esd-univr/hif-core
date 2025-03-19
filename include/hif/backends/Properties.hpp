/// @file Properties.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include <iostream>
#include <map>
#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace backends
{

/// @brief Parameters structure description : key = value
class Properties
{
public:
    /// @brief Load a configuration file
    /// @param config_file_name : Configuration file name
    /// @param silent : = 1 => if no file corresponds => no error message
    /// @param evaluate : = 1 => elaborate values (which depend on others keys)
    void load(const std::string &config_file_name, const char silent = 0, const char evaluate = 1);

    /// @brief Dump the properties list (key = value)
    /// Print on the ostream
    void dump(std::ostream &) const;

    Properties();

    ~Properties();

    Properties(const Properties &o)
        : name(o.name)
        , props(o.props)
    {
    }

    /// @brief Set a key (property)
    Properties &operator=(const Properties &o)
    {
        name  = o.name;
        props = o.props;
        return *this;
    }

    /// @brief Get a property value
    /// @param property_name : key to identify
    /// @return the corresponding value as a string
    std::string getProperty(const std::string &property_name) const
    {
        Properties *This = const_cast<Properties *>(this); // mutable not supported by SUN 5.0
        return This->props[property_name];
    }

    /// @brief Get a property value
    /// @param property_name : key to identify
    /// @return the corresponding value as a string
    std::string operator[](const std::string &property_name) const;

    /// @brief Set a property value
    /// @param property_name : key to find/set
    /// @param property : value to set
    /// @param evaluate : = 1 => elaborate the value
    void setProperty(const std::string &property_name, const std::string &property, const char evaluate = 1);

    /// @brief Append a property
    /// @param property_name : key to append
    /// @param property : value to set
    /// @param evaluate : = 1 => elaborate the value
    void appendProperty(const std::string &property_name, const std::string &property, const char evaluate = 1);

    /// @brief Elaborate a value and return this evaluation
    std::string eval(const std::string &) const;

private:
    /// @brief Configuration file name
    mutable std::string name;
    /// @brief Properties map
    mutable std::map<std::string, std::string, std::less<std::string>> props;
};

} // namespace backends
} // namespace hif
