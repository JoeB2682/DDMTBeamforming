# ========================================================
#
# GenerateFIRCorrection.py
#
# Adds FIR_Correction to existing samples
#
# AI generated class to add correction to the dataset
#
# ========================================================

import os
import numpy as np

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
DATACREATION_FOLDER = os.path.join(BASE_DIR, "SimandDataCreation")

# Training Data Directory
TRAINING_FOLDER = os.path.join(DATACREATION_FOLDER, "TrainingData")

def generate_correction(fir, beam_error):

    """
    Generates FIR correction from beam error.

    fir shape:
        (Frames, Speakers, Taps)

    beam_error shape:
        (Frames, Frequencies, Angles)

    """

    frames, speakers, taps = fir.shape


    # ------------------------------------------------
    # Reduce beam error into a correction strength
    # ------------------------------------------------

    error_strength = np.mean(
        beam_error,
        axis=1
    )


    # Normalise error

    error_strength /= (
        np.max(error_strength) + 1e-8
    )


    # ------------------------------------------------
    # Create correction
    # ------------------------------------------------

    correction = np.zeros_like(fir)


    for frame in range(frames):

        correction[frame,:,:] = (
            -0.05 *
            error_strength[frame]
            *
            fir[frame,:,:]
        )


    return correction.astype(np.float32)

for file in os.listdir(TRAINING_FOLDER):

    if file.endswith(".npz"):

        path = os.path.join(
            TRAINING_FOLDER,
            file
        )


        data = dict(
            np.load(
                path,
                allow_pickle=True
            )
        )


        fir = data["FIR_Coefficients"]

        beam_error = data["Beam_error"]

        print("FIR shape:", fir.shape)
        print("Beam error shape:", beam_error.shape)

        correction = generate_correction(
            fir,
            beam_error
        )

        data["FIR_Correction"] = correction


        np.savez(
            path,
            **data
        )


        print(
            "Updated:",
            file,
            correction.shape
        )