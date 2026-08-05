# ========================================================
# 
# Data-Driven Augmentation of Geometry Based
# Beamforming for Motion Tracked Audio.
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

# ========================================================
# Constants (hyperparameters)
BATCH_SIZE = 128
NO_EPOCHS = 1
LEARNINGRATE = 0.01
DEVICE = "cpu"
# ========================================================
# Instanciation Station
trainingHelper = TrainingHelper("cpu", NO_EPOCHS)
datasethandler = DatasetHandler (TRAINING_FOLDER);
beamnet = BeamNet()
# ========================================================
# Main Running Function
if __name__ == "__main__":
    
    # Get training data
    train_data = datasethandler
    print("Training data downloaded")

    # Create Data Loader for Trainset
    train_data_loader = DataLoader(train_data, batch_size=BATCH_SIZE)    

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
    t.save(beamnet.state_dict(), "BeamNet.pth")
    print("Model trained and stored at BeamNet.pth")
# ========================================================