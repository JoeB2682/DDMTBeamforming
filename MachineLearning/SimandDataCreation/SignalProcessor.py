# ========================================================
# SignalProcessor.py
#
# Handles the creation of signals to be emmitted in room 
# simulations. Also calculates Time-of-Arrival Vectors
# for emmitter phases and various other signal 
# processing functions. 
#
# Created by: Joseph Bozzo
# ========================================================
import pyroomacoustics as PA
import numpy as np
# ========================================================
class SignalProcessor:
    # ====================================================
    # Create Noise Source Signal
    def create_noise_signal(self, length):
        source_signal = np.random.randn(length)
        return source_signal
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
    def fix_audio_length(self, audio):
        
        target_length = self.fs

        if audio.shape[1] > target_length:
            audio = audio[:, :target_length]
        else:
            padding = target_length - audio.shape[1]
            audio = np.pad(audio, ((0,0), (0,padding)))

        return audio
# ========================================================
        