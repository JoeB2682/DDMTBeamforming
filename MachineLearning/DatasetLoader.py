# ========================================================
# 
# DatasetLoader.py
#
# Dataset Handling Class
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
from torch.utils.data import Dataset
# ========================================================
# Model Class
class DatasetHandler(Dataset):
    # ====================================================
    # Constructor
    def __init__(self, Data_dir):

        # Loads entire dataset from path
        self.Data_files = [
            os.path.join(Data_dir, f)
            for f in os.listdir(Data_dir)
            if f.endswith(".npz")
        ]

    # ====================================================
    # Returns Length of the Dataset
    def __len__(self): # dunder method, essentially just override in C++
        return len(self.Data_files)
    
    # ====================================================
    # Gets items from the dataset
    def __getitem__(self, idx):

        data = np.load(
            self.Data_files[idx],
            allow_pickle=True
        )

        room = np.array([
            data["room_dimensions"][0],
            data["room_dimensions"][1],
            data["room_dimensions"][2],
            data["absorption"],
            data["rt60"],
            data["max_order"]
        ], dtype=np.float32)

        trajectory = np.concatenate([
            data["listener_start_pos"],
            data["trajectory"].mean(axis=0),
            data["trajectory"].std(axis=0),
            data["listener_velocity"].mean(axis=0),
            data["listener_velocity"].std(axis=0)
        ])

        # Your MLP expects 35 inputs
        trajectory = trajectory[:35]

        if len(trajectory) < 35:

            trajectory = np.pad(
                trajectory,
                (0,35-len(trajectory))
            )

        trajectory = trajectory.astype(np.float32)

        fir = data["FIR_Coefficients"]
        fir = np.expand_dims(fir, axis=0)
        fir = fir.astype(np.float32)

        beam = data["Beam_Patterns"]
        beam = np.expand_dims(beam, axis=0)
        beam = beam.astype(np.float32)

        correction = data["FIR_Correction"]
        correction = correction.astype(np.float32)

        return (
            t.tensor(room),
            t.tensor(trajectory),
            t.tensor(fir),
            t.tensor(beam),
            t.tensor(correction)
        )
# ========================================================