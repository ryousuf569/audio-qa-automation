#include "include/json.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using json = nlohmann::json;
using namespace std;

struct Thresholds {
    double max_silence_ratio;
    double min_dynamic_range_db;
    double max_noise_zcr_p95;
    double max_brightness_centroid_p95_hz;
    double min_transient_onset_peak_ratio;
    double policy_warn_margin;

    static Thresholds load(const string& path);
};

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

Thresholds Thresholds::load(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Could not open threshold.json");
    }

    json thresholds;
    file >> thresholds;

    return Thresholds{
        thresholds["silence"]["max_ratio"].get<double>(),
        thresholds["dynamic_range"]["min_db"].get<double>(),
        thresholds["noise"]["max_zcr_p95"].get<double>(),
        thresholds["brightness"]["max_centroid_p95_hz"].get<double>(),
        thresholds["transients"]["min_onset_peak_ratio"].get<double>(),
        thresholds["policy"]["warn_margin"].get<double>()
    };
}


vector<Statistics> load_features_csv(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Could not open features CSV");
    }

    vector<Statistics> rows;
    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        Statistics s;

        getline(ss, s.file_id, ',');

        getline(ss, cell, ','); s.rms_mean = stod(cell);
        getline(ss, cell, ','); s.rms_std = stod(cell);
        getline(ss, cell, ','); s.rms_p05 = stod(cell);
        getline(ss, cell, ','); s.rms_p95 = stod(cell);
        getline(ss, cell, ','); s.silence_ratio = stod(cell);
        getline(ss, cell, ','); s.dynamic_range_db = stod(cell);

        getline(ss, cell, ','); s.zcr_mean = stod(cell);
        getline(ss, cell, ','); s.zcr_std = stod(cell);
        getline(ss, cell, ','); s.zcr_p95 = stod(cell);

        getline(ss, cell, ','); s.centroid_mean = stod(cell);
        getline(ss, cell, ','); s.centroid_std = stod(cell);
        getline(ss, cell, ','); s.centroid_p05 = stod(cell);
        getline(ss, cell, ','); s.centroid_p95 = stod(cell);

        getline(ss, cell, ','); s.bandwidth_mean = stod(cell);
        getline(ss, cell, ','); s.bandwidth_std = stod(cell);

        getline(ss, cell, ','); s.onset_mean = stod(cell);
        getline(ss, cell, ','); s.onset_std = stod(cell);
        getline(ss, cell, ','); s.onset_peak_ratio = stod(cell);

        getline(ss, cell, ','); s.flux_mean = stod(cell);
        getline(ss, cell, ','); s.flux_std = stod(cell);
        getline(ss, cell, ','); s.flux_p95 = stod(cell);

        rows.push_back(s);
    }

    return rows;
}