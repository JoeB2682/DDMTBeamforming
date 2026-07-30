# ========================================================
# DatasetManager.py
#
# For saving, viewing and manipulating data samples within
# local datset directory.
#
# Automatically indexes and names data samples upon saving.
#
# Created by: Joseph Bozzo
# ========================================================
import numpy as np
import os
# ========================================================
# Directory Paths

# Training Data Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")

# Beam Plot Directories
PLOT_FOLDER = os.path.join(BASE_DIR, "IdealBeamPlots")
FIR_PLOT_FOLDER = os.path.join(BASE_DIR, "FIRBeamplots")
# ========================================================
class DatasetManager:
    # ====================================================
    # Save Data Sample (uses os to index and name properly)
    def save_sample(self, audio, room_data):

        # use prior defined function to get next sample
        sample_number = self.get_next_sample_number()

        # create filename
        filename = f"sample_{sample_number:05d}.npz"

        # specify file path
        filepath = os.path.join(TRAINING_FOLDER, filename)

        # Compress to reduce file size
        np.savez_compressed(filepath, audio=audio, **room_data)

        print("Saved:", filename)
    # ====================================================
    # Save Ideal Beam Plot For Sample 
    def save_ideal_beamplot(self, plot):

        sample_number = self.get_next_sample_number()

        filename = f"sample_{sample_number:05d}.png"
        filepath = os.path.join(PLOT_FOLDER, filename)

        plot.savefig(filepath, dpi=300, bbox_inches="tight")

        print("Saved:", filename)
    # ====================================================
    # Save Filtered Beam Plot For Sample 
    def save_filtered_beamplot(self, plot):

        sample_number = self.get_next_sample_number()
    
        filename = f"sample_{sample_number:05d}.png"
        filepath = os.path.join(FIR_PLOT_FOLDER, filename)
    
        plot.savefig(filepath, dpi=300, bbox_inches="tight")
    
        print("Saved:", filename)
    # ====================================================
    # Get Next Sample Number (for automatic indexing in folder)
    def get_next_sample_number(self):
        # locate folder
        files = os.listdir(TRAINING_FOLDER)

        # get blank array of sample numbers
        sample_numbers = []

        # iterate files
        for file in files:
            # find last file and increment with naming convention
            if file.startswith("sample_") and file.endswith(".npz"):
                number = file.replace("sample_", "")
                number = number.replace(".npz", "")
                sample_numbers.append(int(number))

        if len(sample_numbers) == 0:
            return 0

        return max(sample_numbers) + 1
    # ====================================================
    # Print Data Sample (pretty looking data printing)
    def print_sample(self, sample_no):

        # Specify sample name using input variable
        filename = f"sample_{sample_no:05d}.npz"

        # Pickle controls serialisation / deserialisation (print as is)
        data = np.load(os.path.join(TRAINING_FOLDER, filename), allow_pickle=True)

        print("\n================================")
        print(f"        {filename}")
        print("================================\n")

        # Print each data entry
        for key in data.files:

            value = data[key]

            print(f"{key}")
            print("--------------------------------")

            if np.isscalar(value) or value.ndim == 0:
                print(f"Value: {value.item()}")

            if key == "rir":
                print(f"Number of RIRs: {value.shape[1]}")

            else:

                print(f"Shape: {value.shape}")

                if value.size <= 100:
                    print("Value:")
                    print(value)

            print()
        print("================================\n")
# ========================================================
        