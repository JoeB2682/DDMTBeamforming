# ========================================================
# 
# BeamNet.py
#   
# Main Neural Network Model
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

from spatial_cnn import SpatialCNN
from temporal_cnn import TemporalCNN
from spectral_cnn import SpectralCNN
# ========================================================
# Model Class
class BeamNet(nn.Module):
    # ====================================================
    # Constructor
    def __init__(self):

        super().__init__()

        self.timecnn = TemporalCNN()
        self.speccnn = SpectralCNN()
        self.spacecnn = SpatialCNN()

        self.fc = nn.Sequential(
            nn.Linear(96,256),
            nn.ReLU(),
            nn.Linear(256,4096)
        )
    # ====================================================
    # Define Propogation Through Network
    def forward(self,audio,stft,beam):

        a = self.time(audio)
        s = self.spec(stft)
        b = self.space(beam)

        x = t.cat([a,s,b],dim=1)

        return self.fc(x)
# ========================================================