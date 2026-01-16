#include <string>
#include <algorithm>
#include <iostream>


using namespace std;

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
};

/* Helpers */
MetricStatus evaluate_upper_bound(double value, double threshold, double warn_margin) {
    if (value <= threshold) {
        return MetricStatus::OK;
    }
    if (value <= threshold * (1.0 + warn_margin)) {
        return MetricStatus::WARN;
    }
    return MetricStatus::FAIL;
}

MetricStatus evaluate_lower_bound(double value, double threshold, double warn_margin) {
    if (value >= threshold) {
        return MetricStatus::OK;
    }
    if (value >= threshold * (1.0 - warn_margin)) {
        return MetricStatus::WARN;
    }
    return MetricStatus::FAIL;
}

/* ---------- Metrics ---------- */

MetricResult analyze_silence(
    const Statistics& stats,
    const Thresholds& thresholds
) {
    return {
        "SilenceRatio",
        stats.silence_ratio,
        evaluate_upper_bound(
            stats.silence_ratio,
            thresholds.max_silence_ratio,
            thresholds.policy_warn_margin
        )
    };
}

MetricResult analyze_dynamic_range(
    const Statistics& stats,
    const Thresholds& thresholds
) {
    return {
        "DynamicRange",
        stats.dynamic_range_db,
        evaluate_lower_bound(
            stats.dynamic_range_db,
            thresholds.min_dynamic_range_db,
            thresholds.policy_warn_margin
        )
    };
}

MetricResult analyze_noise(
    const Statistics& stats,
    const Thresholds& thresholds
) {
    return {
        "NoiseZCR",
        stats.zcr_p95,
        evaluate_upper_bound(
            stats.zcr_p95,
            thresholds.max_noise_zcr_p95,
            thresholds.policy_warn_margin
        )
    };
}

MetricResult analyze_brightness(
    const Statistics& stats,
    const Thresholds& thresholds
) {
    return {
        "Brightness",
        stats.centroid_p95,
        evaluate_upper_bound(
            stats.centroid_p95,
            thresholds.max_brightness_centroid_p95_hz,
            thresholds.policy_warn_margin
        )
    };
}

MetricResult analyze_transients(
    const Statistics& stats,
    const Thresholds& thresholds
) {
    return {
        "TransientStrength",
        stats.onset_peak_ratio,
        evaluate_lower_bound(
            stats.onset_peak_ratio,
            thresholds.min_transient_onset_peak_ratio,
            thresholds.policy_warn_margin
        )
    };
}