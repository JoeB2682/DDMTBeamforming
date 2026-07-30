# ========================================================
# BeamEvaluator.py
#
# Applies FIR coeffs to the generated beam pattern...
#
# Used AI to Generate the majority of this class due to 
# needing it quickly...
#
# Created by: Joseph Bozzo
# ========================================================
import os
import numpy as np
import matplotlib.pyplot as plt
# ========================================================
# Constants and directories
SAMPLERATE = 48000

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")
# ========================================================
class BeamEvaluator:
    # ====================================================
    def evaluate_fir_beam(
        self,
        speaker_positions,
        fir_coefficients,
        angles,
        listener_positions  
    ):

        num_frames = fir_coefficients.shape[0]
        num_speakers = speaker_positions.shape[1]

        c = 343

        # Frequency bins for FIR response
        freqs = np.linspace(200, 1900, 32)

        beam_patterns = []

        # Array centre
        array_center = np.mean(speaker_positions, axis=1)

        # Precompute FIR time indices (N samples)
        filter_len = fir_coefficients.shape[2] if fir_coefficients.ndim == 3 else len(fir_coefficients[0][0])
        n = np.arange(filter_len)

        # Evaluate each trajectory frame
        for frame in range(num_frames):

            listener_pos = listener_positions[frame]

            listener_radius = np.linalg.norm(
                listener_pos - array_center
            )

            coeffs = fir_coefficients[frame]
            angle_response = []

            # Scan beam angles
            for angle in angles:

                theta = np.deg2rad(angle)
                direction = np.array([np.cos(theta), np.sin(theta), 0])

                # Near-field scan point set at actual listener distance
                scan_point = array_center + direction * listener_radius

                frequency_responses = []

                # Evaluate across frequency range
                for freq in freqs:

                    k = 2 * np.pi * freq / c
                    output = 0.0 + 0.0j

                    # Vectorized FIR response kernel per frequency
                    # H(f) = sum( h[n] * exp(-j * 2*pi * f * n / Fs) )
                    fir_kernel = np.exp(-1j * 2 * np.pi * freq * n / SAMPLERATE)

                    # Sum contribution from every speaker
                    for speaker in range(num_speakers):

                        speaker_pos = speaker_positions[:, speaker]

                        # Direct distance from speaker to scan point
                        scan_distance = np.linalg.norm(speaker_pos - scan_point)

                        # Standard wave propagation phase lag: exp(-j * k * r)
                        spatial_phase = np.exp(-1j * k * scan_distance)

                        # Complex FIR frequency response H(f) for this speaker
                        h = coeffs[speaker]
                        H = np.sum(h * fir_kernel)

                        #if frame == 0 and speaker == 0:
                        #    print(freq, np.angle(H))

                        # Total complex pressure contribution at scan point
                        output += H * spatial_phase

                    frequency_responses.append(np.abs(output))

                # Average wideband response across frequencies
                angle_response.append(np.mean(frequency_responses))
            beam_patterns.append(angle_response)
        beam_patterns = np.array(beam_patterns)

        # Normalise to dB
        beam_patterns = (20 * np.log10(beam_patterns /(np.max(beam_patterns) + 1e-12)+ 1e-12 ))

        return beam_patterns
# ========================================================