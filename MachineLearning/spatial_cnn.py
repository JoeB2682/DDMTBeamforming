# ========================================================
# 
# spatial_cnn.py
#   
# CNN for spatial feature extraction
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
class SpatialCNN(nn.Module):
    # ====================================================
    # Constructor
    def __init__(self):

        super().__init__()

        self.fc = nn.Sequential(
            nn.Linear(96,256),
            nn.ReLU(),
            nn.Linear(256,4096)
        )
    # ====================================================
    # Define Propogation Through Network
    def forward(self): pass

# ========================================================