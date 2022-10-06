#pragma once

#include <fstream>
#include <vector>
#include <map>
#include "v_user.h"

#ifdef USE_NLOHMANNJSON

#include <nlohmann/json.hpp>

class JsonConfigSection
{
public:
    JsonConfigSection(const std::string& filename);

    explicit JsonConfigSection(nlohmann::json arg_json):
        obj_json(arg_json)
    {}

    template <typename T>
    T get_as() const {
        return obj_json.get<T>();
    }

    bool contains(const std::string& name_token ) const
    {
        return obj_json.contains(name_token);
    }

    bool is_string() const
    {
        return obj_json.is_string();
    }

    std::optional<JsonConfigSection> section(const std::string &variable_name_path) const;
    std::vector<JsonConfigSection> to_vector();
    std::string to_string() const
    {
        return obj_json.dump();
    }

protected:
    nlohmann::json obj_json;  // Store a pointer to the config file

};

#else

#include <QJsonObject>
#include <optional>

class JsonConfigSection
{
public:
    JsonConfigSection(const std::string& filename);

    explicit JsonConfigSection(const QJsonValue& arg_json):
        obj_json(arg_json)
    {}

    template <class T>
    T get_as() const {
        if(std::is_integral_v<T>) {
            return static_cast<T>(obj_json.toInt());
        }
        else {
            return static_cast<T>(obj_json.toDouble());
        }
    }

    bool contains(const std::string& name_token ) const
    {
        return obj_json.toObject().contains(QString::fromStdString(name_token));
    }

    bool is_string() const
    {
        return obj_json.isString();
    }

    std::optional<JsonConfigSection> section(const std::string &variable_name_path) const;
    std::string to_string() const;
    std::vector<JsonConfigSection> to_vector();

protected:
    QJsonValue obj_json;

};

template <>  std::string JsonConfigSection::get_as<std::string>() const;
template <>  bool JsonConfigSection::get_as<bool>() const ;
template <>  std::map<std::string, std::vector<std::string>> JsonConfigSection::get_as<std::map<std::string, std::vector<std::string>>>() const;
template <>  std::vector<std::string> JsonConfigSection::get_as<std::vector<std::string>>() const;

#endif

/// Definition of json based section settings
class TJsonConfig
{
    JsonConfigSection section_object;

public:

    TJsonConfig(const std::string& filename):
        section_object(filename)
    {}

    explicit TJsonConfig(JsonConfigSection arg_json):
        section_object(arg_json)
    {}

    std::optional<TJsonConfig> section(const string &variable_name_path) const
    {
        auto opt_result =  section_object.section(variable_name_path);
        if (!opt_result) {
            return std::nullopt;
        }
        return TJsonConfig(*opt_result);
    }

    /// Check if a section object contains a certain jsonpath.
    bool contains(const std::string& variable_name_path) const
    {
        std::optional<TJsonConfig> opt_result = section(variable_name_path);
        if (!opt_result) {
            return false;
        }
        return true;
    }

    bool is_string() const
    {
        return section_object.is_string();
    }

    template <typename T>
    T get_as() const
    {
        return section_object.get_as<T>();
    }

    template <typename T>
    std::optional<T> value(const std::string& variable_name_path) const
    {
        std::optional<TJsonConfig> opt_result =  section(variable_name_path);
        if (!opt_result) {
            return std::nullopt;
        }
        return opt_result->get_as<T>();
    }

    template <typename T>
    T value_or_default( const std::string& variable_name_path, T default_value) const
    {
        std::optional<T> maybe = value<T>(variable_name_path);
        return maybe.value_or(default_value);
    }

    template <typename T>
    T value_must_exist(const std::string& variable_name_path) const
    {
        std::optional<T> maybe = value<T>(variable_name_path);
        if (!maybe) {
            Error( variable_name_path, "The variable is missing.");
        }
        return *maybe;
    }

    std::vector<TJsonConfig> to_vector()
    {
        auto section_vector = section_object.to_vector();
        std::vector<TJsonConfig> config_as_vector;
        std::transform(
                    section_vector.begin(),
                    section_vector.end(),
                    std::back_inserter(config_as_vector),
                    [](JsonConfigSection item) { return TJsonConfig(item); });
        return config_as_vector;
    }

    /// @brief Dump section to JSON string.
    std::string dump() const
    {
        return  section_object.to_string();
    }

};


