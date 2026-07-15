# ========================================================
# ArrayCreator.py
#
# Creates either circular or linear arrays.
#
# Created by: Joseph Bozzo
# ========================================================
import pyroomacoustics as PA
import numpy as np
# ========================================================
class ArrayCreator:
    # ====================================================
    # Create Speaker Positions (Horizontal Linear Array)
    def create_linear_speaker_positions(self, num_speakers, spacing, height):

        speaker_positions = np.zeros((3, num_speakers))
        centre = (num_speakers - 1) / 2

        for i in range(num_speakers):

            speaker_positions[0, i] = ((i - centre) * spacing)
            speaker_positions[1, i] = 0
            speaker_positions[2, i] = height

        return speaker_positions
    # ====================================================
    # Create Circular Speaker Array Positions
    def create_cirular_speaker_array(self, num_speakers, radius, centre, height):

        speaker_positions = np.zeros((3, num_speakers))

        for i in range(num_speakers):

            angle = (2 * np.pi * i / num_speakers)

            speaker_positions[0, i] = (centre[0] + radius * np.cos(angle))
            speaker_positions[1, i] = (centre[1] + radius * np.sin(angle))
            speaker_positions[2, i] = height

        return speaker_positions
# ========================================================
        