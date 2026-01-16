// g++ computation/main.cpp computation/ingestion.cpp computation/analysis.cpp -o audio_qa
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

struct Statistics {
    string file_id;

    double rms_mean;
    double rms_std;
    double rms_p05;
    double rms_p95;
    double silence_ratio;
    double dynamic_range_db;

    double zcr_mean;
    double zcr_std;
    double zcr_p95;

    double centroid_mean;
    double centroid_std;
    double centroid_p05;
    double centroid_p95;

    double bandwidth_mean;
    double bandwidth_std;

    double onset_mean;
    double onset_std;
    double onset_peak_ratio;

    double flux_mean;
    double flux_std;
    double flux_p95;
};

struct Thresholds {
    double max_silence_ratio;
    double min_dynamic_range_db;
    double max_noise_zcr_p95;
    double max_brightness_centroid_p95_hz;
    double min_transient_onset_peak_ratio;
    double policy_warn_margin;

    static Thresholds load(const string& path);
};

vector<Statistics> load_features_csv(const string& path);

enum class MetricStatus {
    OK,
    WARN,
    FAIL
};

struct MetricResult {
    string metric_name;
    double value;
    MetricStatus status;
};

MetricResult analyze_silence(const Statistics&, const Thresholds&);
MetricResult analyze_dynamic_range(const Statistics&, const Thresholds&);
MetricResult analyze_noise(const Statistics&, const Thresholds&);
MetricResult analyze_brightness(const Statistics&, const Thresholds&);
MetricResult analyze_transients(const Statistics&, const Thresholds&);


string status_to_string(MetricStatus status) {
    switch (status) {
        case MetricStatus::OK:   return "OK";
        case MetricStatus::WARN: return "WARN";
        case MetricStatus::FAIL: return "FAIL";
        default:                 return "UNKNOWN";
    }
}

int main() {
    try {
        Thresholds thresholds = Thresholds::load("config/threshold.json");

        vector<Statistics> files =
            load_features_csv("data/processed/stats.csv");

        ofstream output("data/processed/computation_results.csv");
        if (!output.is_open()) {
            throw runtime_error("Could not open output CSV");
        }

        output << "file_id,metric,value,status\n";

        for (const auto& stats : files) {

            vector<MetricResult> results = {
                analyze_silence(stats, thresholds),
                analyze_dynamic_range(stats, thresholds),
                analyze_noise(stats, thresholds),
                analyze_brightness(stats, thresholds),
                analyze_transients(stats, thresholds)
            };

            for (const auto& r : results) {
                output
                    << stats.file_id << ","
                    << r.metric_name << ","
                    << r.value << ","
                    << status_to_string(r.status)
                    << "\n";
            }
        }

        output.close();
        cout << "Analysis complete. Results written to analysis_results.csv\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
