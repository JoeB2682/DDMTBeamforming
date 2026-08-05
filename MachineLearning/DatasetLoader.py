# ========================================================
# 
# DatasetLoader.py
#
# Dataset Handling Class, loads and uses set within main 
# script
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
    # Pads FIR data to maximum speaker size, this is needed 
    # due to random size generation 
    def pad_speakers(self, data, max_speakers):

        padded_data = np.zeros(
            (data.shape[0],
             max_speakers,
             data.shape[2]),
            dtype=np.float32
        )

        padded_data[:, :data.shape[1], :] = data

        return padded_data
    # ====================================================
    # Gets items from the dataset
    def __getitem__(self, idx):

        data = np.load(
            self.Data_files[idx],
            allow_pickle=True
        )

        #print("Room dimensions:", data["room_dimensions"].shape)
        #print("Absorption:", data["absorption"])
        #print("RT60:", data["rt60"], data["rt60"].shape)
        #print("Max order:", data["max_order"])
        #print("Speakers:", data["num_speakers"])
        #exit()

        room = np.array([
            data["room_dimensions"][0],
            data["room_dimensions"][1],
            data["room_dimensions"][2],
            data["absorption"],
            np.mean(data["rt60"]), # rt60 needs averaging
            data["max_order"],
            data["num_speakers"]
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

        # FIR Coefficients
        # Original shape = (Frames, Speakers, Taps)

        fir = data["FIR_Coefficients"]
        fir = self.pad_speakers(fir, max_speakers=24)
        fir = np.expand_dims(fir, axis=0).astype(np.float32)

        beam = data["Beam_Patterns"]
        beam = np.expand_dims(beam, axis=0)
        beam = beam.astype(np.float32)

        # FIR Correction Target
        # Original shape = (Frames, Speakers, Taps)
        correction = data["FIR_Correction"]
        correction = self.pad_speakers(correction, max_speakers=24)
        correction = correction.astype(np.float32)

        filtered_beam = data["FIR_Beam_Patterns"]
        filtered_beam = np.expand_dims(
            filtered_beam,
            axis=0
        ).astype(np.float32)

        # Return Loaded Data as Tensors
        return (
            t.tensor(room),
            t.tensor(trajectory),
            t.tensor(fir),
            t.tensor(beam),
            t.tensor(filtered_beam),
            t.tensor(correction)
        )
# ========================================================