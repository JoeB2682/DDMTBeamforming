# ========================================================
#
# TestModel.py
#
# Tests to see model I/O sizes and whether it infers...
#
# Primary references for this process:
# [https://brsoff.github.io/tutorials/advanced/cpp_export.html](https://brsoff.github.io/tutorials/advanced/cpp_export.html)
# https://docs.pytorch.org/tutorials/beginner/onnx/export_simple_model_to_onnx_tutorial.html
#
# Due to libtorch being a pain to integrate into C++ due 
# to not being able to locate c10 lib and clashing with 
# defined Steinberg Macros. I have have to switch to ONNX,
# hence the annoying saving path thing. 
#
# Created by: Joseph Bozzo
#
# ========================================================
# Imports
import os
import torch as t

from BeamNet import BeamNet
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
MODEL_FORMAT = "onnx"
# ========================================================

if __name__ == "__main__":

    # Specify path
    modelname = f"BeamNet_{MODEL_TO_LOAD:05d}.pth"
    modelpath = os.path.join(MODELS_FOLDER, modelname)

    # Load model
    model = BeamNet()

    checkpoint = t.load(
        modelpath,
        map_location="cpu",
        weights_only=True
    )

    model.load_state_dict(checkpoint)

    # Set model to evaluation mode
    model.eval()
    #print(model)
    # ====================================================
    # Test inference

    batch_size = 1

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

    print(f"\nOutput size:")
    print(f"Output: ", output.shape)

    # ====================================================
    # Print output
    print(f"\nOutput values:")
    print(output)
    # This deduces that the model is indeed infering 
    # coefficient corrections correctly :)
    # ====================================================
    # Save model

    # Save ONNX model (.onnx)
    if MODEL_FORMAT == "onnx":

        savepath = os.path.join(
            MODELS_FOLDER,
            f"BeamNet_ONNX_{MODEL_TO_LOAD:05d}.onnx"
        )

        t.onnx.export(
            model,
            (
                room,
                trajectory,
                fir,
                beam,
                filtered_beam
            ),
            savepath,
            input_names=[
                "room",
                "trajectory",
                "fir",
                "beam",
                "filtered_beam"
            ],
            output_names=[
                "output"
            ]
        )

    # Save Pytorch model (.pt)
    elif MODEL_FORMAT == "pt":
        savepath = os.path.join(
            MODELS_FOLDER,
            f"BeamNet_{MODEL_TO_LOAD:05d}.pth"
        )
        t.save(model.state_dict(), savepath)

    else:
        # Get angry if not specified save format
        raise ValueError(f"Unsupported MODEL_FORMAT: {MODEL_FORMAT}")
# ========================================================