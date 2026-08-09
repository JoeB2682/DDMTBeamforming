# ========================================================
#
# TestModel.py
#
# Tests to see model I/O sizes and whether it infers...
#
# Primary reference for this process:
# https://brsoff.github.io/tutorials/advanced/cpp_export.html
#
# Created by: Joseph Bozzo
#
# ========================================================
# Imports
import os
import torch as t
# ========================================================
# Directories

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Saved Models Directory
MODELS_FOLDER = os.path.join(BASE_DIR, "Models")

# Serialised model directory
SMODELS_FOLDER = os.path.join(MODELS_FOLDER, "Serialised")

# ========================================================
# Constants
MODEL_TO_LOAD = 1
# ========================================================
if __name__ == "__main__":

    # Specify path
    modelname = f"BeamNet_Script_{MODEL_TO_LOAD:05d}.pt"
    modelpath = os.path.join(SMODELS_FOLDER, modelname)

    # Load model
    model = t.jit.load(modelpath)

    # Set model to evaluation mode
    model.eval()

    print(model)

    # ====================================================
    # Test inference

    # Batch size
    batch_size = 1

    # Create inputs
    room = t.randn(batch_size, 7)
    trajectory = t.randn(batch_size, 35)
    fir = t.randn(batch_size, 1, 100, 8, 64)
    beam = t.randn(batch_size, 1, 72, 100)
    filtered_beam = t.randn(batch_size, 1, 72, 100)

    # ====================================================
    # Run inference
    with t.no_grad():
        output = model(
            room,
            trajectory,
            fir,
            beam,
            filtered_beam
        )
    # ====================================================
    # Print I/O sizes
    print("\nInput sizes:")
    print(f"Room: ", room.shape)
    print(f"Trajectory: ", trajectory.shape)
    print(f"FIR: ", fir.shape)
    print(f"Beam: ", beam.shape)
    print(f"Filtered Beam: ", filtered_beam.shape)

    print(f"\nOutput size: ")
    print(f"Output: ", output.shape)
    # ====================================================
    # Print output 
    print(f"\nOutput values:")
    print(output)

    # This deduces that the model is indeed infering 
    # coefficient corrections correctly :)
# ========================================================