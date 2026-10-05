#include "config.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace {
std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

bool set_value(Config& c, const std::string& key, const std::string& value) {
    try {
        if (key == "meter_id") c.meter_id = value;
        else if (key == "pulse_energy_wh") c.pulse_energy_wh = std::stod(value);
        else if (key == "normal_min_watts") c.normal_min_watts = std::stod(value);
        else if (key == "normal_max_watts") c.normal_max_watts = std::stod(value);
        else if (key == "anomaly_spike_watts") c.anomaly_spike_watts = std::stod(value);
        else if (key == "anomaly_drop_watts") c.anomaly_drop_watts = std::stod(value);
        else if (key == "samples") c.samples = static_cast<std::size_t>(std::stoull(value));
        else if (key == "seed") c.seed = static_cast<unsigned>(std::stoul(value));
        else if (key == "interval_seconds") c.interval_seconds = static_cast<unsigned>(std::stoul(value));
        else if (key == "telemetry_file") c.telemetry_file = value;
        else if (key == "event_file") c.event_file = value;
        else if (key == "summary_file") c.summary_file = value;
        else return false;
    } catch (...) {
        return false;
    }
    return true;
}
}

bool load_config(const std::string& path, Config& config, std::string& error) {
    std::ifstream in(path);
    if (!in) {
        error = "Unable to open config file: " + path;
        return false;
    }

    std::string line;
    std::size_t line_no = 0;
    while (std::getline(in, line)) {
        ++line_no;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        const auto pos = line.find('=');
        if (pos == std::string::npos) {
            error = "Invalid config line " + std::to_string(line_no);
            return false;
        }

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        if (!set_value(config, key, value)) {
            error = "Invalid or unknown config at line " + std::to_string(line_no);
            return false;
        }
    }
    return true;
}
