# ========================================================
# 
# TrainingFunction.py
#   
# General class to help with training Neural Networks
#
# Created by: Joseph Bozzo
#    
# ========================================================
# Imports
import os
import torch as t
from torch import nn
from torch.utils.data import DataLoader
from torchvision.transforms import ToTensor
from torchvision import datasets
# ========================================================
# Directories

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")

# ========================================================
# Model Class
class TrainingHelper():

    # ====================================================
    # Constructor
    def __init__(self, device, epochs):

        self.device = device
        self.epochs = epochs


    # ====================================================
    # Define one epoch
    def train_one_epoch(self, model, data_loader, loss_fn, optimiser, device):

        model.train()

        for room, trajectory, fir, beam, targets in data_loader:

            # Move data to GPU/CPU
            room = room.to(device)
            trajectory = trajectory.to(device)
            fir = fir.to(device)
            beam = beam.to(device)
            targets = targets.to(device)

            # Forward pass
            predictions = model(
                room,
                trajectory,
                fir,
                beam
            )

            # Calculate loss
            loss = loss_fn(
                predictions,
                targets
            )

            # Backpropagation
            optimiser.zero_grad()
            loss.backward()
            optimiser.step()

        print(f"Loss: {loss.item()}")

    # ====================================================
    # Training Function
    def train(self, model, data_loader, loss_fn, optimiser, device, epochs):

        model.to(device)

        for i in range(epochs):
            print(f"Epoch {i+1}")
            self.train_one_epoch(
                model,
                data_loader,
                loss_fn,
                optimiser,
                device
            )
            print("---------------------")
        print("Training Done Lad...")      
# ========================================================