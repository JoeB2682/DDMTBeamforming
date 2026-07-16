# ========================================================
# SignalProcessor.py
#
# Handles the creation of signals to be emmitted in room 
# simulations. Also calculates Time-of-Arrival Vectors
# for emmitter phases and various other signal 
# processing functions. 
#
# References for sourced speech and Music Samples:
#
# - 
#
# Created by: Joseph Bozzo
# ========================================================
import os
import pyroomacoustics as PA
import numpy as np
import random
import soundfile as sf
from scipy.signal import resample_poly
# ========================================================
# Specify Sample and base dirs
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

SPEECH_FOLDER = os.path.join(BASE_DIR, "SpeechSamples")
MUSIC_FOLDER = os.path.join(BASE_DIR, "MusicSamples")
BG_NOISE_FOLDER = os.path.join(BASE_DIR, "BackgroundNoiseSamples")
# ========================================================
class SignalProcessor:
    # ====================================================
    # Create Noise Source Signal
    def create_noise_signal(self, length):
        source_signal = np.random.randn(length)
        return source_signal
    # ====================================================
    # Create Narrowband Signal
    def create_narrowband(self, freq, length):

        # Creates equal spacing within the signal array
        t = np.arange(length) / self.fs

        # equation for sine wave
        source_signal = np.sin(2*np.pi*freq*t)

        return source_signal

    # ====================================================
    # Create Wideband Chord Signal
    def create_wideband_chord(self, f1, f2, f3, length):

        # Creates equal spacing within the signal array
        t = np.arange(length) / self.fs

        # Create 3 individual pure tones from input freqs
        s_1 = np.sin(2*np.pi*f1*t)
        s_2 = np.sin(2*np.pi*f2*t)
        s_3 = np.sin(2*np.pi*f3*t)

        # sum and normalise
        source_signal = (s_1+s_2+s_3) / 3

        return source_signal
    # ====================================================
    # Pick random Speech sample from dir as source
    def use_speech_signal(self):

        files = [
            f for f in os.listdir(SPEECH_FOLDER)
            if f.endswith(".wav") or f.endswith(".mp3")
        ]

        random_file = random.choice(files)
        file_path = os.path.join(SPEECH_FOLDER, random_file)

        audio, fs = sf.read(file_path)

        # force mono if needed
        if audio.ndim > 1:
            audio = audio[:, 0]
        elif fs != 48000:
            # resample if not at 48kHz
            audio = resample_poly(audio, 48000, fs)

        return audio, random_file
    # ====================================================
    # Pick Music sample from dir as source
    def use_music_signal(self):

        files = [
            f for f in os.listdir(MUSIC_FOLDER)
            if f.endswith(".wav") or f.endswith(".mp3")
        ]

        random_file = random.choice(files)
        file_path = os.path.join(MUSIC_FOLDER, random_file)

        audio, fs = sf.read(file_path)

        # force mono if needed
        if audio.ndim > 1:
            audio = audio[:, 0]
        elif fs != 48000:
            # resample if not at 48kHz
            audio = resample_poly(audio, 48000, fs)

        return audio, random_file
    # ====================================================
    # Get Background Noise Sample from dir
    def get_bg_noise_sample(self, level):

        files = [
            f for f in os.listdir(BG_NOISE_FOLDER)
            if f.endswith(".wav") or f.endswith(".mp3")
        ]

        random_file = random.choice(files)
        file_path = os.path.join(BG_NOISE_FOLDER, random_file)

        audio, fs = sf.read(file_path)

        # force mono if needed
        if audio.ndim > 1:
            audio = audio[:, 0]
        elif fs != 48000:
            # resample if not at 48kHz
            audio = resample_poly(audio, 48000, fs)

        audio = audio * level

        return audio, random_file

    # ====================================================
    # Calculate Time-Of-Arrival (from speakers to listener)
    def calculate_TOA(self, speaker_positions, listener_position):
        # Pretty much the same as MATLAB and C++ 
        c = 343  # speed of sound (m/s)
        tau = []

        for speaker in speaker_positions.T:
            distance = np.linalg.norm(
                speaker - listener_position
            )
            delay = distance / c
            tau.append(delay)

        return np.array(tau)
    # ====================================================
    # Apply Phase Delay to Source (like the beamformer) 
    def apply_delay(self, signal, delay):

        samples = int(delay * self.fs)
        delayed_signal = np.pad(signal, (samples, 0))
        delayed_signal = delayed_signal[:len(signal)]

        return delayed_signal
    # ====================================================
    # Fix Audio Length (ensures generated audio is the same length),
    # fixes using sample rate specified
    def fix_audio_length(self, audio, length):
        
        target_length = self.fs * length

        if audio.shape[1] > target_length:
            audio = audio[:, :target_length]
        else:
            padding = target_length - audio.shape[1]
            audio = np.pad(audio, ((0,0), (0,padding)))

        return audio
# ========================================================
        