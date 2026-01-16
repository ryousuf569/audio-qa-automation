import pandas as pd
import numpy as np

"""
mean std summarize the central tendency and variability of frame-level audio featuress
percentiles (5th and 95th) provide estimates of feature extremes while reducing sensitivity to outliers
these statistics collapse time-based audio measurements into stable, file-level metrics for further analysiss
"""

def percentile(series, q):
    # a helper computes a percentile value for a numeric pandas Series
    return np.percentile(series.dropna(), q)


# The rest of these functions calculate standard values for every statistic we got from
# our librosa code provided by the last project
def energy_stats(df, silence_threshold=0.001):
    rms = df["energy"]

    return {
        "rms_mean": rms.mean(),
        "rms_std": rms.std(),
        "rms_p05": percentile(rms, 5),
        "rms_p95": percentile(rms, 95),
        "silence_ratio": (rms < silence_threshold).mean(),
        "dynamic_range_db": 20 * np.log10(
            percentile(rms, 95) / max(percentile(rms, 5), 1e-6)
        )
    }

def noise_stats(df):
    zcr = df["noise"]

    return {
        "zcr_mean": zcr.mean(),
        "zcr_std": zcr.std(),
        "zcr_p95": percentile(zcr, 95)
    }

def brightness_stats(df):
    centroid = df["brightness"]

    return {
        "centroid_mean": centroid.mean(),
        "centroid_std": centroid.std(),
        "centroid_p05": percentile(centroid, 5),
        "centroid_p95": percentile(centroid, 95)
    }

def bandwidth_stats(df):
    bw = df["bandwidth"]

    return {
        "bandwidth_mean": bw.mean(),
        "bandwidth_std": bw.std()
    }

def flux_stats(df):
    flux = df["spectrum_changes"]

    return {
        "flux_mean": flux.mean(),
        "flux_std": flux.std(),
        "flux_p95": percentile(flux, 95)
    }

def onset_stats(df):
    onset = df["onset"]

    mean_onset = onset.mean()
    max_onset = onset.max()

    return {
        "onset_mean": mean_onset,
        "onset_std": onset.std(),
        "onset_peak_ratio": max_onset / max(mean_onset, 1e-6)
    }

def extract_file_level_stats(df, silence_threshold=0.001):
    # This is a series that we feed to our C++ engine
    stats = {}

    stats.update(energy_stats(df, silence_threshold))
    stats.update(noise_stats(df))
    stats.update(brightness_stats(df))
    stats.update(bandwidth_stats(df))
    stats.update(onset_stats(df))
    stats.update(flux_stats(df))

    return pd.Series(stats)


df = pd.read_csv("data/processed/tempdata.csv")
df = df.drop('Unnamed: 0', axis = 1)
series = extract_file_level_stats(df)
stat_df = pd.DataFrame(series).T
stat_df['file_id'] = df['file_path']
stat_df = stat_df[['file_id'] + [col for col in stat_df.columns if col != 'file_id']]
stat_df.to_csv("data/processed/stats.csv", index=False)