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
from TrainingLogger import TrainingLogger
# ========================================================
# Directories

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")

# .csv File Directory
TRAINING_LOG = os.path.join(BASE_DIR, "TrainingLog")

# ========================================================
# Model Class
class TrainingHelper():

    # ====================================================
    # Constructor
    def __init__(self, device, epochs, learning_rate):

        self.device = device
        self.epochs = epochs
        self.learning_rate = learning_rate

    # ====================================================
    # Define one epoch
    def train_one_epoch(self, model, data_loader, loss_fn, optimiser, device):

        model.train()

        total_loss = 0.0

        for room, trajectory, fir, beam, filtered_beam, correction in data_loader:

            # Move data to GPU/CPU
            room = room.to(device)
            trajectory = trajectory.to(device)
            fir = fir.to(device)
            beam = beam.to(device)
            filtered_beam = filtered_beam.to(device)
            correction = correction.to(device)

            predicted_correction = model(
                room,
                trajectory,
                fir,
                beam,
                filtered_beam
            )

            # Calculate Loss Betwwen Predicted and Correction
            loss = loss_fn(
                predicted_correction,
                correction
            )

            # Backpropagation
            optimiser.zero_grad()
            loss.backward()
            optimiser.step()

            # Accumulate Loss
            total_loss += loss.item()

        # Average Loss Across Batches
        average_loss = total_loss / len(data_loader)

        # Current Batch Loss
        current_loss = loss.item()

        # Print Loss
        print(f"Current Batch Loss: {loss.item()}")
        print(f"Average Loss: {average_loss}")

        # Return Average
        return average_loss, current_loss

    # ====================================================
    # Training Function
    def train(self, model, data_loader, loss_fn, optimiser, device, epochs):

        model.to(device)

        # Determine Training Attempt Number (based on no. of CSVs in dir)
        csv_files = [ 
            file for file in os.listdir(TRAINING_LOG) 
            if file.lower().endswith(".csv") 
        ]

        training_attempt = len(csv_files) + 1

        # Create logger 
        training_logger = TrainingLogger(training_attempt, self.learning_rate)

        try:

            for i in range(epochs):

                print(f"Epoch {i+1}")

                average_loss, current_loss = self.train_one_epoch(
                    model,
                    data_loader,
                    loss_fn,
                    optimiser,
                    device
                )

                # Write epoch result to .csv
                training_logger.Writelineofdata(
                    i + 1,
                    current_loss,
                    average_loss,
                    self.learning_rate
                )
                print("---------------------")

        finally:

            # Close CSV file when training finishes 
            training_logger.close()

        print("Training Done Lad...")
# ========================================================