#pragma once

#include "analyzer.hpp"
#include <string>
#include <vector>

bool initialize_telemetry(const std::string& telemetry_file,
                          const std::string& event_file,
                          std::string& error);

bool append_reading(const std::string& telemetry_file,
                    const MeterReading& reading,
                    std::string& error);

bool append_event(const std::string& event_file,
                  const AnalysisEvent& event,
                  std::string& error);

bool write_summary_json(const std::string& path,
                        const AnalyticsSummary& summary,
                        std::string& error);
