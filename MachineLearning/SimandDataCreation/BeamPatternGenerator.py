# ========================================================
# BeamPatternGenerator.py
#
# Generates ideal narrowband beam directivity.
#
# Used AI to Generate the majority of this class due to 
# needing it quickly....
#
# I saw comeone on linkdin using pyargus for beam plotting
# in python so though it was a good idea...
#
# Even though i'm not just using narrowband, this will assume 
# narrowband but will still allow rough directivity estimation
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
class BeamPatternGenerator:

    # ====================================================
    # Generate Ideal Beampattern
    def generate_beam_pattern(self,
                              array_positions,
                              freq,
                              listener_pos):

        c = 343

        # Array centre
        array_center = np.mean(
            array_positions,
            axis=1
        )

        # Listener direction
        listener_vector = (
            listener_pos - array_center
        )

        listener_angle = np.arctan2(
            listener_vector[1],
            listener_vector[0]
        )

        # Steering direction towards listener
        steering_direction = np.array([
            np.cos(listener_angle),
            np.sin(listener_angle),
            0
        ])

        # Get actual radial distance to the listener for near-field scan
        listener_radius = np.linalg.norm(
            listener_vector
        )

        # Scan angles
        scan_angles = np.linspace(
            -np.pi,
            np.pi,
            361
        )

        response = []

        # Check if the array is linear (all Y coordinates are virtually identical)
        is_linear = np.allclose(array_positions[1, :], array_positions[1, 0])

        # Generate pattern
        for angle in scan_angles:

            direction = np.array([
                np.cos(angle),
                np.sin(angle),
                0
            ])

            beam = 0

            for speaker in array_positions.T:

                # Steering delay towards listener (unchanged)
                listener_distance = np.linalg.norm(listener_pos - speaker)
                steering_delay = listener_distance / c

                # FIX: Check geometry type for scan points
                if is_linear:
                    # For a linear array, use a far-field projection 
                    # to keep phase relationships coherent across the line
                    scan_point = array_center + (direction * 100.0)
                else:
                    # For a circular array, stick to the near-field radius
                    scan_point = array_center + (direction * listener_radius)

                scan_distance = np.linalg.norm(scan_point - speaker)
                scan_delay = scan_distance / c

                # Narrowband phase response
                beam += np.exp(
                    -1j *
                    2 *
                    np.pi *
                    freq *
                    (steering_delay - scan_delay)
                )

            response.append(
                np.abs(beam)
            )

        response = np.array(response)

        # Normalise
        response /= np.max(response)

        # Convert to dB
        response_db = 20*np.log10(
            response + 1e-12
        )

        return (
            np.rad2deg(scan_angles),
            response_db,
            np.rad2deg(listener_angle)
        )
    
    # ====================================================
    # Plot Pattern
    def plot_beam_pattern(self,
                      angles,
                      response,
                      listener_angle):
    
        theta = np.deg2rad(angles)

        fig = plt.figure(
            figsize=(8,8)
        )

        ax = fig.add_subplot(111, polar=True)

        ax.plot(
            theta,
            response
        )

        ax.axvline(
            np.deg2rad(listener_angle),
            linestyle="--",
            label="Listener direction"
        )

        ax.set_title(
            "Ideal Narrowband Beam Directivity Pattern"
        )

        ax.legend()

        return fig
# ========================================================