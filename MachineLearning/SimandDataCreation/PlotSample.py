# ========================================================
# PlotSample.py
#
# Plots a generated room sample from the dataset.
#
# Used ChatGPT for this as I don't have the time to learn
# the Python lib...
#
# Created by: Joseph Bozzo
# ========================================================
import os
import numpy as np
import matplotlib.pyplot as plt
# ========================================================
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")
# ========================================================
class Sample_Plotter():
    # ====================================================
    # Plot Data Sample
    def plot_sample(self, sample_no):

        filename = f"sample_{sample_no:05d}.npz"

        filepath = os.path.join(TRAINING_FOLDER, filename)

        data = np.load(filepath, allow_pickle=True)

        room = data["room_dimensions"]

        speakers = data["speaker_positions"]
        listener = data["listener_position"]
        mic = data["mic_positions"]
        bg_noise = data["Background_noise_position"]

        plt.figure(figsize=(7, 5))

        # Room boundary
        plt.xlim(0, room[0])
        plt.ylim(0, room[1])

        # Speakers
        plt.scatter(
            speakers[0, :],
            speakers[1, :],
            marker="s",
            label="Speakers"
        )

        # Listener
        plt.scatter(
            listener[0],
            listener[1],
            marker="x",
            s=100,
            label="Listener"
        )

        # Microphone
        plt.scatter(
            mic[0, :],
            mic[1, :],
            marker="o",
            label="Microphone"
        )

        # Background Noise
        plt.scatter(
            bg_noise[0],
            bg_noise[1],
            marker="x",
            label="BGNoise"
        )

        # Label speakers
        for i in range(speakers.shape[1]):
            plt.text(
                speakers[0, i],
                speakers[1, i],
                str(i)
            )

        plt.xlabel("x (m)")
        plt.ylabel("y (m)")
        plt.title(f"Sample {sample_no}")
        plt.legend()
        plt.grid()

        plt.show()
# ========================================================