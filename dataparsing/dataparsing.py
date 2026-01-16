import pandas as pd
import numpy as np
import librosa

def load_stem(path, sr=22050, mono=True):
        # y is us loading the waveform and sr is sample rate
        y, sr = librosa.load(path, sr=sr, mono=mono)
        y = librosa.util.normalize(y)
        return y, sr

def stem_features(y, sr, hop_length=2048):

        # STFT magnitude, represents a signal in the time-frequency domain
        S = np.abs(librosa.stft(y, n_fft=2048, hop_length=hop_length))

        rms = librosa.feature.rms(S=S)[0] # perceived loudness
        centroid = librosa.feature.spectral_centroid(S=S, sr=sr)[0] # brightness
        bandwidth = librosa.feature.spectral_bandwidth(S=S, sr=sr)[0] # spread
        zcr = librosa.feature.zero_crossing_rate(y, hop_length=hop_length)[0] # noisy/edgy

        # how “eventful”
        onset_env = librosa.onset.onset_strength(y=y, sr=sr, hop_length=hop_length)
        # how fast the spectrum changes
        flux = librosa.onset.onset_strength(
            S=librosa.power_to_db(S**2),
            sr=sr,
            hop_length=hop_length)
        
        times = librosa.frames_to_time(
            np.arange(len(rms)), sr=sr, hop_length=hop_length)

        return {
        "times": times,
        "energy": rms,
        "brightness": centroid,
        "bandwidth": bandwidth,
        "noise": zcr,
        "onset": onset_env,
        "spectrum_changes": flux, }

path = "data/raw/disco.00000.wav"
y, sr = load_stem(path)
features = stem_features(y, sr)
df = pd.DataFrame(features)
df['file_path'] = path

df.to_csv("data/processed/tempdata.csv")