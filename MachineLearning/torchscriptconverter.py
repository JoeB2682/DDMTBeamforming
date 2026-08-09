# ========================================================
# 
# torchscriptconverter.py
#
# To use models generated from the training stages in C++...
# they must be converted to torchscript so they can be 
# executed as binary files.
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
import torchvision

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
MODEL_TO_CONV = 1
# ========================================================
# Training Logger Class
class ModelLoader:
    # ====================================================
    # Constructor
    def __init__(self, modeldir, modelno):

        # Specify directories and which model to load
        self.modeldir = modeldir 
        self.modelno = modelno 

    # ====================================================
    # Load model
    def load_model(self, model):

        # Construct model filename
        modelname = f"BeamNet_{self.modelno:05d}.pth"
        modelpath = os.path.join(self.modeldir, modelname)

        # Select GPU if available, otherwise CPU
        device = t.device("cuda" if t.cuda.is_available() else "cpu")

        # Load model weights
        model.load_state_dict(t.load(modelpath, map_location=device))

        # Move model to device
        model.to(device)

        return model
    # ====================================================
    # Script Conversion
    def script_converter(self, model):

        # Sets model to evaluation mode (as we're not tra9ining now)
        model.eval()

        # Convert the model
        scripted_model = t.jit.script(model)

        return scripted_model
    # ====================================================
    # Save Scripted Model
    def save_scripted(self, scripted_model):

        # Specify Path
        modelname = f"BeamNet_Script_{self.modelno:05d}.pt"
        modelpath = os.path.join(SMODELS_FOLDER, modelname)

        # Save script model
        scripted_model.save(modelpath)

# ========================================================
# I'm keeping this one on one script...
if __name__ == "__main__":

    # Instanciate Loader
    loader = ModelLoader(MODELS_FOLDER, MODEL_TO_CONV)

    # Instanciate empty model
    beamnet = BeamNet()

    # Load weights into model
    beamnet = loader.load_model(beamnet)

    # Convert to TorchScript
    scripted_model = loader.script_converter(beamnet)

    # Save model script
    loader.save_scripted(scripted_model)
# ========================================================
