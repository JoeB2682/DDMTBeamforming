# ========================================================
# 
# BeamNet.py
#   
# Main Convolutional-Neural-Network-Model
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
class BeamNet(nn.Module):
    # ====================================================
    # Constructor
    def __init__(self):

        super().__init__()
        # ================================================
        # Layers
        # ================================================

        # ==========================================================================================================================
        # Branch 1

        # Multi-Layer-Perceptron for room features (Output = number of learned feature representations, weight tuning)
        self.room_mlp = nn.Sequential(nn.Linear(in_features=6, out_features=32), # Dimensions(L,W,H), Absorption, RT60, MaxOrder
                                      nn.ReLU(), # Linear Activation

                                      nn.Linear(in_features=32, out_features=64), 
                                      nn.ReLU(),

                                      nn.Linear(in_features=64, out_features=32), 
                                      nn.ReLU(),

                                      nn.Linear(in_features=32, out_features=32), 
                                      nn.ReLU())

        # MLP for Trajectory Data
        self.room_mlp = nn.Sequential(nn.Linear(in_features=35, out_features=64), # Start Pos, Trajectory, Velocity 
                                              nn.ReLU(), 
        
                                              nn.Linear(in_features=64, out_features=64), 
                                              nn.ReLU())

        # Input FIR Coefficients to first convolutional layer (coeffs have 3 dimensions)
        self.conv_1 = nn.Conv3d(in_channels=1, # Number of inputs to layer
                               out_channels=16, # Number of Outputs of layer
                               kernel_size=(3,3,5), # Size of convolution kernel (x by x by x), (bins, speaker, taps)
                               padding=(1,1,2)) # Prevents feature map shrinking
        # ==========================================================================================================================
        # Branch 2

        # Input the Ideal Beam Pattern 
        self.conv_2 = nn.Conv2d(in_channels=1,
                               out_channels=16,
                               kernel_size=(3,3),
                               padding=1)


        
    # ====================================================
    # Define Propogation Through Network
    def forward(self):

        # a = self.time(audio)
        # s = self.spec(stft)
        # b = self.space(beam)

        #output = t.cat([a,s,b],dim=1)

        #return self.fc(output)
        pass 
# ========================================================