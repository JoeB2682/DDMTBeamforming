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

        self.Data_files = [
            os.path.join(Data_dir, f)
            for f in os.listdir(Data_dir)
            if f.endswith(".npz")
        ]

    # ====================================================
    # Returns Length of the Dataset
    def __len__(self): # dunder method, essentially just override in C++
        return len(self.Data_files)
# ========================================================