#include "include/json.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;
using namespace std;

struct Thresholds {
    double max_silence_ratio;
    double min_dynamic_range_db;
    double max_noise_zcr_p95;
    double max_brightness_centroid_p95_hz;
    double min_transient_onset_peak_ratio;
    double policy_warn_margin;

    static Thresholds load(const string& path) {
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
};

int main(){
    Thresholds thresholds = Thresholds::load("C:/Users/ryous/Downloads/audio-qa-automation/config/threshold.json");
}
