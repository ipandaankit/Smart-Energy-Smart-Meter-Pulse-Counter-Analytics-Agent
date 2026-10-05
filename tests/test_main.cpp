#include "analyzer.hpp"
#include "meter.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

int main() {
    {
        SmartMeter meter(1.0);

        auto r1 = meter.process_pulse(make_pulse(1, 1000, 1.0));
        auto r2 = meter.process_pulse(make_pulse(2, 1001, 1.0));

        assert(r1.total_pulses == 1);
        assert(r2.total_pulses == 2);
        assert(std::abs(r2.cumulative_energy_kwh - 0.002) < 1e-9);
        assert(std::abs(r2.estimated_power_w - 3600.0) < 1e-9);
    }

    {
        Analyzer analyzer(20.0, 5000.0, 7000.0, 5.0);

        MeterReading high{1, 1, 1, 2.0, 0.002, 8000.0};
        MeterReading low{2, 2, 2, 0.001, 0.002001, 2.0};
        MeterReading normal{3, 3, 3, 1.0, 0.003001, 1000.0};

        assert(analyzer.analyze(high).type == EventType::HighConsumption);
        assert(analyzer.analyze(low).type == EventType::LowConsumption);
        assert(analyzer.analyze(normal).type == EventType::Normal);
    }

    {
        Analyzer analyzer(20.0, 5000.0, 7000.0, 5.0);
        std::vector<MeterReading> readings{
            {1, 1, 1, 1.0, 0.001, 1000.0},
            {2, 2, 2, 2.0, 0.003, 8000.0}
        };
        std::vector<AnalysisEvent> events{
            analyzer.analyze(readings[0]),
            analyzer.analyze(readings[1])
        };

        auto s = analyzer.summarize(readings, events);
        assert(s.samples == 2);
        assert(s.high_events == 1);
        assert(std::abs(s.total_energy_kwh - 0.003) < 1e-9);
        assert(std::abs(s.peak_power_w - 8000.0) < 1e-9);
    }

    std::cout << "All tests passed.\n";
    return 0;
}
