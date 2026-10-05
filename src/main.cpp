#include "analyzer.hpp"
#include "config.hpp"
#include "meter.hpp"
#include "telemetry.hpp"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace {
void print_usage(const char* program) {
    std::cout
        << "Usage: " << program << " [options]\n\n"
        << "Options:\n"
        << "  --config <file>       Configuration file (default: config/smart_meter.conf)\n"
        << "  --samples <n>         Override number of simulated samples\n"
        << "  --seed <n>            Override random seed\n"
        << "  --realtime             Wait interval_seconds between samples\n"
        << "  --help                 Show this help\n";
}

double simulated_power(std::size_t i, std::mt19937& rng) {
    std::normal_distribution<double> noise(0.0, 80.0);
    double base = 900.0 + 350.0 * std::sin(static_cast<double>(i) / 12.0);
    double power = std::max(50.0, base + noise(rng));

    // Inject deterministic events so the demo visibly exercises analytics.
    if (i == 25 || i == 26) power = 8200.0;
    if (i == 70) power = 2.0;

    return power;
}
}

int main(int argc, char* argv[]) {
    std::string config_path = "config/smart_meter.conf";
    bool realtime = false;
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        } else if (arg == "--samples" && i + 1 < argc) {
            config.samples = static_cast<std::size_t>(std::stoull(argv[++i]));
        } else if (arg == "--seed" && i + 1 < argc) {
            config.seed = static_cast<unsigned>(std::stoul(argv[++i]));
        } else if (arg == "--realtime") {
            realtime = true;
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            print_usage(argv[0]);
            return 2;
        }
    }

    std::string error;
    Config file_config;
    if (!load_config(config_path, file_config, error)) {
        std::cerr << "Config error: " << error << '\n';
        return 1;
    }

    // Command-line overrides take precedence.
    if (config.samples == 120) config.samples = file_config.samples;
    if (config.seed == 42) config.seed = file_config.seed;
    file_config.samples = config.samples;
    file_config.seed = config.seed;
    config = file_config;

    try {
        SmartMeter meter(config.pulse_energy_wh);
        Analyzer analyzer(config.normal_min_watts,
                          config.normal_max_watts,
                          config.anomaly_spike_watts,
                          config.anomaly_drop_watts);

        if (!initialize_telemetry(config.telemetry_file, config.event_file, error)) {
            std::cerr << "Telemetry error: " << error << '\n';
            return 1;
        }

        std::mt19937 rng(config.seed);
        std::vector<MeterReading> readings;
        std::vector<AnalysisEvent> events;
        readings.reserve(config.samples);
        events.reserve(config.samples);

        const auto start = std::chrono::system_clock::now();
        const auto start_epoch =
            std::chrono::duration_cast<std::chrono::seconds>(
                start.time_since_epoch()).count();

        std::cout << "Smart Energy Smart-Meter Analytics Agent\n";
        std::cout << "Meter: " << config.meter_id << '\n';
        std::cout << "Samples: " << config.samples << '\n';
        std::cout << "----------------------------------------\n";

        for (std::size_t i = 0; i < config.samples; ++i) {
            const double power_w = simulated_power(i, rng);

            // Convert power over one interval into Wh.
            const double energy_wh =
                power_w * static_cast<double>(config.interval_seconds) / 3600.0;

            const std::uint64_t timestamp =
                static_cast<std::uint64_t>(start_epoch) +
                i * config.interval_seconds;

            Pulse pulse = make_pulse(
                static_cast<std::uint64_t>(i + 1),
                timestamp,
                energy_wh
            );

            MeterReading reading = meter.process_pulse(pulse);
            AnalysisEvent event = analyzer.analyze(reading);

            if (!append_reading(config.telemetry_file, reading, error) ||
                !append_event(config.event_file, event, error)) {
                std::cerr << "Output error: " << error << '\n';
                return 1;
            }

            readings.push_back(reading);
            events.push_back(event);

            if (event.type != EventType::Normal) {
                std::cout << "[EVENT] " << event.message << '\n';
            }

            if (realtime) {
                std::this_thread::sleep_for(
                    std::chrono::seconds(config.interval_seconds));
            }
        }

        AnalyticsSummary summary = analyzer.summarize(readings, events);
        if (!write_summary_json(config.summary_file, summary, error)) {
            std::cerr << "Summary error: " << error << '\n';
            return 1;
        }

        std::cout << "\nSimulation complete.\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Total energy:  " << summary.total_energy_kwh << " kWh\n";
        std::cout << "Average power: " << summary.average_power_w << " W\n";
        std::cout << "Peak power:    " << summary.peak_power_w << " W\n";
        std::cout << "High events:   " << summary.high_events << '\n';
        std::cout << "Low events:    " << summary.low_events << '\n';
        std::cout << "Telemetry:     " << config.telemetry_file << '\n';
        std::cout << "Events:        " << config.event_file << '\n';
        std::cout << "Summary:       " << config.summary_file << '\n';

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
