#include "run_loglevel.h"

#include <array>
#include <map>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <spdlog/spdlog.h>
#include <GEMS3K/jsonconfig.h>

#include "v_module.h"

namespace RunLogLevel {

namespace {

const std::array<const char*, 2> keys = { "level_equilibrium", "level_process" };
const std::array<const char*, 8> gems3k_loggers =
    { "gems3k", "ipm", "ipmlog", "tnode", "kinmet", "solmod", "thermofun", "chemicalfun" };

std::array<std::string, 2> levels = { "default", "default" };
std::map<std::string, spdlog::level::level_enum> baseline;

QJsonObject readConfig()
{
    QFile file(QString::fromStdString(GemsSettings::settings_file_name));
    if(!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return QJsonDocument::fromJson(file.readAll()).object();
}

} // namespace

void init()
{
    auto log_section = readConfig().value("log").toObject();
    for(size_t ii = 0; ii < keys.size(); ++ii) {
        auto name = log_section.value(keys[ii]).toString("default").toStdString();
        levels[ii] = name.empty() ? "default" : name;
    }
    baseline.clear();
    for(const auto* name : gems3k_loggers) {
        if(auto logger = spdlog::get(name)) {
            baseline[name] = logger->level();
        }
    }
}

std::string level(Mode mode)
{
    return levels[mode];
}

void apply(Mode mode)
{
    const auto& name = levels[mode];
    if(name != "default") {
        gemsSettings().gems3k_update_log_level(static_cast<size_t>(spdlog::level::from_str(name)));
        return;
    }
    for(const auto& [lname, lev] : baseline) {
        if(auto logger = spdlog::get(lname)) {
            logger->set_level(lev);
        }
    }
}

bool setLevel(Mode mode, const std::string& name)
{
    levels[mode] = name;
    apply(mode);

    auto root = readConfig();
    auto log_section = root.value("log").toObject();
    log_section[keys[mode]] = QString::fromStdString(name);
    root["log"] = log_section;

    QFile file(QString::fromStdString(GemsSettings::settings_file_name));
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        gui_logger->warn("Unable to save log level to {}", GemsSettings::settings_file_name);
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

} // namespace RunLogLevel
