#pragma once

#include <string>

/// Log level of the GEMS3K loggers for the two calculation modes.
/// Stored in gemsgui-config.json as log.level_equilibrium and log.level_process.
/// An empty/"default" value keeps the level given by the rest of the "log" section.
namespace RunLogLevel {

enum Mode { Equilibrium = 0, Process = 1 };

/// Reads both levels from the settings file and remembers the loggers' current levels as the baseline.
void init();

/// Level name of the mode: "default", "trace", "debug", "info", "warn", "err", "critical" or "off".
std::string level(Mode mode);

/// Sets the level of the mode, applies it to the loggers and saves it to the settings file.
/// Returns false if the file could not be written (the level is still applied).
bool setLevel(Mode mode, const std::string& name);

/// Applies the level of the mode to the GEMS3K loggers (restores the baseline for "default").
void apply(Mode mode);

} // namespace RunLogLevel
