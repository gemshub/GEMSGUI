#include <QVariant>
#include "jsonconfig.h"

#ifdef USE_NLOHMANNJSON

JsonConfigSection::JsonConfigSection(const string &filename):
    obj_json(nlohmann::json::parse(std::ifstream(filename)))
{}

std::vector<JsonConfigSection> JsonConfigSection::to_vector()
{
    if (!obj_json.is_array())
    {
        Error( "to_vector", "Must be array type variable.");
    }
    std::vector<JsonConfigSection> json_as_vector;
    std::transform(
                obj_json.begin(),
                obj_json.end(),
                std::back_inserter(json_as_vector),
                [](nlohmann::json item) { return JsonConfigSection(item); });
    return json_as_vector;
}

std::optional<JsonConfigSection> JsonConfigSection::section(const string &variable_name_path) const
{
    std::vector<std::string> name_tokens = split(variable_name_path, ".");
    nlohmann::json json_element = obj_json;
    for (const std::string& name_token : name_tokens)
    {
        if (!contains(name_token))
        {
            return std::nullopt;
        }
        json_element = json_element.at(name_token);
    }
    return JsonConfigSection(json_element);
}

#else

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

JsonConfigSection::JsonConfigSection(const string &filename)
{
    QFile jsonFile(QString::fromStdString(filename));
    jsonFile.open(QFile::ReadOnly);
    auto doc = QJsonDocument().fromJson(jsonFile.readAll());
    if( doc.isArray() ) {
        obj_json = QJsonValue(doc.array());
    }
    else {
        obj_json = QJsonValue(doc.object());
    }
}

std::string JsonConfigSection::to_string() const
{
    if( obj_json.isArray() ) {
        return QJsonDocument(obj_json.toArray()).toJson().toStdString();
    }
    else if( obj_json.isObject() ) {
         return QJsonDocument(obj_json.toObject()).toJson().toStdString();
    }
    return obj_json.toString().toStdString();
}

std::vector<JsonConfigSection> JsonConfigSection::to_vector()
{
    if (!obj_json.isArray())
    {
        Error( "to_vector", "Must be array type variable.");
    }
    QJsonArray json_array = obj_json.toArray();
    std::vector<JsonConfigSection> json_as_vector;
    std::transform(
                json_array.begin(),
                json_array.end(),
                std::back_inserter(json_as_vector),
                [](const QJsonValue& item) { return JsonConfigSection(item); });
    return json_as_vector;
}

std::optional<JsonConfigSection> JsonConfigSection::section(const string &variable_name_path) const
{
    std::vector<std::string> name_tokens = split(variable_name_path, ".");
    QJsonValue json_element = obj_json;
    for (const auto& name_token : name_tokens)
    {
        QJsonValue json_val = json_element[QString::fromStdString(name_token)];
        if(json_val.isUndefined())
        {
            return std::nullopt;
        }
        json_element = json_val;
    }
    return JsonConfigSection(json_element);
}


template <>  std::string JsonConfigSection::get_as<std::string>() const {
    return obj_json.toString().toStdString();
}

template <>  bool JsonConfigSection::get_as<bool>() const {
    return obj_json.toBool();
}

template<>
std::map<string, std::vector<string>> JsonConfigSection::get_as<std::map<std::string, std::vector<std::string>>>() const
{
    std::map<string, std::vector<string>> data_map;
    auto qmap = obj_json.toVariant().toMap();
    auto qmap_iter = qmap.begin();

    while (qmap_iter != qmap.end()) {
        QList<QVariant> value_list =  qmap_iter.value().toList();
        std::vector<string> list_data;
        auto list_iter = value_list.begin();
        while (list_iter != value_list.end()) {
           list_data.push_back(list_iter->toString().toStdString());
           list_iter++;
        }
        data_map[qmap_iter.key().toStdString()] = list_data;
        qmap_iter++;
    }
    return data_map;
}

template<>
std::vector<string> JsonConfigSection::get_as<std::vector<std::string>>() const
{
    QList<QVariant> value_list =  obj_json.toVariant().toList();
    std::vector<string> list_data;
    auto list_iter = value_list.begin();
    while (list_iter != value_list.end()) {
        list_data.push_back(list_iter->toString().toStdString());
        list_iter++;
    }
    return list_data;
}

#endif


