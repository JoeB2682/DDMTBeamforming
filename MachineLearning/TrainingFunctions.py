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
import torch as t
from torch import nn
from torch.utils.data import DataLoader
from torchvision.transforms import ToTensor
from torchvision import datasets
# ========================================================
# Model Class
class TrainingHelper():
    # ====================================================
    # Constructor
    def __init__(self):

        super().__init__()

    # ====================================================
    # Get Training and Validation Datasets
    def download_Datasets(self):

        # Training Data
        #train_data = 

        #Validation_data = 

        pass

    # ====================================================
    # Define one epoch
    def train_one_epoch(self, model, data_loader, loss_fn, optimiser, device):

        #    for inputs, targets in data_loader:
        #        inputs, targets = inputs.to(device), targets.to(device)
        #
        #        # Calculate Loss
        #        predictions = model(inputs)
        #        loss = loss_fn(predictions, targets)
        #
        #        # Backpropogate Loss Update Weights
        #        optimiser.zero_grad() # Reset gradients upon each training pass/ batch
        #        loss.backward()
        #        optimiser.step()
        #
        #    print(f"Loss: {loss.item()}")

        pass

    # ====================================================
    # Training Function
    def train(self, model, data_loader, loss_fn, optimiser, device, epochs):
    
        #    for i in range(epochs):
        #        print(f"Epoch {i+1}")
        #        train_one_epoch(model, data_loader, loss_fn, optimiser, device)
        #        print("---------------------")
        #
        #    print("Training Done Lad...")    
      
        pass     

# ========================================================