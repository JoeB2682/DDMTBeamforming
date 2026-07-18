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
        listener_vector = listener_pos - array_center

        listener_angle = np.arctan2(
            listener_vector[1],
            listener_vector[0]
        )

        # Listener distance changes every trajectory frame
        listener_radius = np.linalg.norm(
            listener_vector
        )

        # Scan angles
        scan_angles = np.linspace(
            -np.pi,
            np.pi,
            72
        )

        # Directions for every scan angle
        directions = np.stack(
            [
                np.cos(scan_angles),
                np.sin(scan_angles),
                np.zeros_like(scan_angles)
            ],
            axis=1
        )

        # Determine array type
        is_linear = np.allclose(
            array_positions[1, :],
            array_positions[1, 0]
        )

        # ------------------------------------------------
        # Steering delays (depends on listener position)
        # Recalculated every trajectory frame
        # ------------------------------------------------

        listener_distances = np.linalg.norm(
            array_positions.T - listener_pos,
            axis=1
        )

        steering_delays = listener_distances / c

        # ------------------------------------------------
        # Create scan points for all angles
        # ------------------------------------------------

        if is_linear:
            scan_points = (
                array_center
                +
                directions * 100.0
            )
        else:
            scan_points = (
                array_center
                +
                directions * listener_radius
            )

        # ------------------------------------------------
        # Distance from every scan point to every speaker
        #
        #  Shape:
        #     (angles, speakers)
        # ------------------------------------------------

        scan_distances = np.linalg.norm(
            scan_points[:, None, :]
            -
            array_positions.T[None, :, :],
            axis=2
        )

        scan_delays = scan_distances / c

        # ------------------------------------------------
        # Phase response
        #
        # angles x speakers
        # ------------------------------------------------

        phase = np.exp(
            -1j *
            2 *
            np.pi *
            freq *
            (
                steering_delays[None, :]
                -
                scan_delays
            )
        )

        # Sum speakers
        beam = np.sum(
            phase,
            axis=1
        )

        response = np.abs(beam)

        # Normalise
        response /= np.max(response)

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