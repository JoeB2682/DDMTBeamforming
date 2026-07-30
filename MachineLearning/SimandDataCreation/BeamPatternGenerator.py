# ========================================================
# BeamPatternGenerator.py
#
# Generates ideal wideband beam directivity.
#
# Used AI to Generate the majority of this class due to
# needing it quickly....
#
# Created by: Joseph Bozzo
# ========================================================
import numpy as np
import matplotlib.pyplot as plt
# ========================================================
class BeamPatternGenerator:

    # ====================================================
    # Generate Ideal Wideband Beam Pattern
    def generate_beam_pattern(
        self,
        array_positions,
        frequencies,
        listener_pos
    ):
        c = 343
        
        # Array centre
        array_center = np.mean(array_positions, axis=1)

        # Listener direction
        listener_vector = (listener_pos - array_center)
        listener_angle = np.arctan2(listener_vector[1], listener_vector[0])

        # Listener distance
        listener_radius = np.linalg.norm(listener_vector)

        # Scan angles
        scan_angles = np.linspace(-np.pi, np.pi, 72)

        directions = np.stack(
            [
                np.cos(scan_angles),
                np.sin(scan_angles),
                np.zeros_like(scan_angles)
            ],
            axis=1
        )

        # Steering delays
        # Delay required for listener position
        listener_distances = np.linalg.norm(
            array_positions.T -
            listener_pos,
            axis=1
        )

        steering_delays = (listener_distances / c)

        # Create near-field scan points
        scan_points = (
            array_center + directions * listener_radius
        )

        scan_distances = np.linalg.norm(
            scan_points[:, None, :] -
            array_positions.T[None, :, :],
            axis=2
        )

        scan_delays = (scan_distances / c)

        # wideband response
        response = np.zeros(len(scan_angles))

        for freq in frequencies:

            # Steering phase
            phase = np.exp(1j * 2 * np.pi * freq *
                (steering_delays[None, :] - scan_delays)
            )

            # Sum speakers
            beam = np.sum(phase, axis=1)

            # Accumulate power
            response += (np.abs(beam) ** 2)

        # RMS pressure
        response = np.sqrt( response / len(frequencies))

        # Normalise
        response /= (np.max(response) + 1e-12)

        # Convert to dB
        response_db = (20 * np.log10(response + 1e-12))

        return (
            np.rad2deg(scan_angles),
            response_db,
            np.rad2deg(listener_angle)
        )
    
    # ====================================================
    # Plot Beam Pattern
    def plot_beam_pattern(
        self,
        angles,
        response,
        listener_angle,
        title="Beam Directivity Pattern"
    ):
        theta = np.deg2rad(angles)
        fig = plt.figure(figsize=(8,8))

        ax = fig.add_subplot(111, polar=True)
        ax.plot(theta, response)

        ax.axvline(
            np.deg2rad(listener_angle),
            linestyle="--",
            label="Listener direction"
        )

        ax.set_title(title)
        ax.legend()

        return fig
     # ====================================================