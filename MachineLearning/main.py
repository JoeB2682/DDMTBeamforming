# ========================================================
# 
# Data-Driven Augmentation of Geometry Based
# Beamforming for Motion Tracked Audio.
# 
# Note: ensure that the GPU 'cuda' version of torchvision 
# is installed before running this as by default the cpu 
# only version is available. SO DON'T TRAIN ON LAPTOP!!!!!
#   
# Main running script.
#
# Created by: Joseph Bozzo
#    
# ========================================================
# Module Imports
import os
import torch as t
import numpy as np
import pandas as pd

from torch import nn
from torch.utils.data import DataLoader
from torchvision.transforms import ToTensor
from torchvision import datasets

# Header Imports
from TrainingFunctions import TrainingHelper
from DatasetLoader import DatasetHandler
from BeamNet import BeamNet
# ========================================================
# Directories

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
DATACREATION_FOLDER = os.path.join(BASE_DIR, "SimandDataCreation")

# Training Data Directory
TRAINING_FOLDER = os.path.join(DATACREATION_FOLDER, "TrainingData")

# Saved Models Directory
MODELS_FOLDER = os.path.join(BASE_DIR, "Models")

# Training Log Directory
TRAINING_LOG = os.path.join(BASE_DIR, "TrainingLog")

# ========================================================
# Constants (hyperparameters)
BATCH_SIZE = 128
NO_EPOCHS = 256
LEARNINGRATE = 0.0001
DEVICE = "cuda" # "cuda" = gpu
# ========================================================
# Instanciation Station
trainingHelper = TrainingHelper(DEVICE, NO_EPOCHS, LEARNINGRATE)
datasethandler = DatasetHandler (TRAINING_FOLDER);
beamnet = BeamNet()
# ========================================================
# Main Running Function
if __name__ == "__main__":

    # Determine Current Training Attempt 
    training_attempt = len([
        file for file in os.listdir("TrainingLog")
        if file.endswith(".csv")
    ]) + 1
    
    # Get training data
    train_data = datasethandler
    print("Training data downloaded")

    # Create Data Loader for Trainset, USE SHUFFLE
    train_data_loader = DataLoader(train_data, batch_size=BATCH_SIZE, shuffle=True)    

    # ====================================================
    # Check Dataset Shapes
    #sample = next(iter(train_data_loader))

    #for item in sample:
    #    print(item.shape)

    # Stop here for debugging
    #exit()
    # ====================================================
    # Instanciate Loss function + Optimiser
    loss_fn = nn.MSELoss()
    optimiser = t.optim.Adam(beamnet.parameters(), lr=LEARNINGRATE)

    # Train Each NNModel (Pass all prior defined shit)
    trainingHelper.train(beamnet, train_data_loader, loss_fn, optimiser, DEVICE, NO_EPOCHS)

    # Store Model
    model_filename = f"BeamNet_{training_attempt:05d}.pth"
    model_path = os.path.join(MODELS_FOLDER, model_filename)

    t.save(beamnet.state_dict(), model_path)

    print(f"Model trained and stored at {model_path}")
# ========================================================