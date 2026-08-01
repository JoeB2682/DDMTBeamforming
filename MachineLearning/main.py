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
from BeamNet import BeamNet
# ========================================================
# Directories

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")

# Validation Data Directory
VALIDATION_FOLDER = os.path.join(BASE_DIR, "ValidationData")

# ========================================================
# Constants (hyperparameters)
BATCH_SIZE = 128
NO_EPOCHS = 10
LEARNINGRATE = 0.01
# ========================================================
# Instanciation Station
trainingHelper = TrainingHelper("cpu", NO_EPOCHS)
beamnet = BeamNet()
# ========================================================
# Main Running Function
if __name__ == "__main__":
    
    print("Main")
    
    # Get training data
    # train_data, _ = 
    #print("Training data downloaded")

    # Possibly print data to validate
    
    # Create Data Loader for Trainset
    #train_data_loader = DataLoader(train_data, batch_size=BATCH_SIZE)    
  
    # Instanciate Loss function + Optimiser
    #loss_fn = MSE()
    #optimiser = torch.optim.Adam(feed_forward_net.parameters(), lr=LEARNINGRATE)

    # Train Each NNModel (Pass all prior defined shit)
    #train(BeamNet, train_data_loader, loss_fn, optimiser, device, NO_EPOCHS)

    # Store Model
    #torch.save(feed_forward_net.state_dict(), "BeamNet.pth")

    #print("Model trained and stored at BeamNet.pth")