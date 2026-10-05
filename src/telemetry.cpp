#include "telemetry.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>

namespace fs = std::filesystem;

namespace {
bool ensure_parent(const std::string& path, std::string& error) {
    try {
        fs::path p(path);
        if (p.has_parent_path()) fs::create_directories(p.parent_path());
        return true;
    } catch (const std::exception& ex) {
        error = ex.what();
        return false;
    }
}
}

bool initialize_telemetry(const std::string& telemetry_file,
                          const std::string& event_file,
                          std::string& error) {
    if (!ensure_parent(telemetry_file, error) ||
        !ensure_parent(event_file, error)) return false;

    std::ofstream t(telemetry_file, std::ios::trunc);
    std::ofstream e(event_file, std::ios::trunc);
    if (!t || !e) {
        error = "Unable to initialize telemetry files";
        return false;
    }

    t << "sequence,timestamp_epoch,total_pulses,interval_energy_wh,cumulative_energy_kwh,estimated_power_w\n";
    e << "sequence,timestamp_epoch,type,power_w,message\n";
    return true;
}

bool append_reading(const std::string& telemetry_file,
                    const MeterReading& r,
                    std::string& error) {
    std::ofstream out(telemetry_file, std::ios::app);
    if (!out) {
        error = "Unable to write telemetry file";
        return false;
    }

    out << r.sequence << ','
        << r.timestamp_epoch << ','
        << r.total_pulses << ','
        << std::fixed << std::setprecision(3)
        << r.interval_energy_wh << ','
        << r.cumulative_energy_kwh << ','
        << r.estimated_power_w << '\n';
    return true;
}

bool append_event(const std::string& event_file,
                  const AnalysisEvent& e,
                  std::string& error) {
    std::ofstream out(event_file, std::ios::app);
    if (!out) {
        error = "Unable to write event file";
        return false;
    }

    const char* type = "NORMAL";
    if (e.type == EventType::HighConsumption) type = "HIGH_CONSUMPTION";
    else if (e.type == EventType::LowConsumption) type = "LOW_CONSUMPTION";
    else if (e.type == EventType::PulseGap) type = "PULSE_GAP";

    out << e.sequence << ','
        << e.timestamp_epoch << ','
        << type << ','
        << std::fixed << std::setprecision(3)
        << e.power_w << ','
        << '"' << e.message << '"' << '\n';
    return true;
}

bool write_summary_json(const std::string& path,
                        const AnalyticsSummary& s,
                        std::string& error) {
    if (!ensure_parent(path, error)) return false;

    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        error = "Unable to write summary file";
        return false;
    }

    out << std::fixed << std::setprecision(3)
        << "{\n"
        << "  \"samples\": " << s.samples << ",\n"
        << "  \"high_consumption_events\": " << s.high_events << ",\n"
        << "  \"low_consumption_events\": " << s.low_events << ",\n"
        << "  \"pulse_gap_events\": " << s.gap_events << ",\n"
        << "  \"total_energy_kwh\": " << s.total_energy_kwh << ",\n"
        << "  \"average_power_w\": " << s.average_power_w << ",\n"
        << "  \"peak_power_w\": " << s.peak_power_w << "\n"
        << "}\n";
    return true;
}
