# ========================================================
# FeatureExtractor.py
#
# Extracts spectral features. 
#
# Created by: Joseph Bozzo
# ========================================================
import numpy as np
import scipy.signal as signal
# ========================================================
# Constants
SAMPLERATE = 48000
FFTSIZE = 2**11
# ========================================================
class FeatureExtractor:
    # ====================================================
    # Extract Spectral Features
    def extract_spectral_features(self, audio):
        
        if audio.ndim == 1:
            audio = audio[np.newaxis, :]

        frequencies, times, spectrum = signal.stft(
            audio[0, :],
            fs=self.fs,
            nperseg=self.fft_size,
            noverlap=self.fft_size // 2
        )

        magnitude = np.abs(spectrum)

        return magnitude
# ========================================================
        