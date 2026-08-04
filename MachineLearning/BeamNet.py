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

        # Multi-Layer-Perceptron for room features (Output = learned room feature representations)
        self.room_mlp = nn.Sequential(nn.Linear(in_features=6, out_features=32), # Dimensions(L,W,H), Absorption, RT60, MaxOrder
                                      nn.ReLU(),

                                      nn.Linear(in_features=32, out_features=64),
                                      nn.ReLU(),

                                      nn.Linear(in_features=64, out_features=32),
                                      nn.ReLU(),

                                      nn.Linear(in_features=32, out_features=32),
                                      nn.ReLU())

        # MLP for Trajectory Data
        self.trajectory_mlp = nn.Sequential(nn.Linear(in_features=35, out_features=64), # Start Pos, Trajectory, Velocity
                                            nn.ReLU(),

                                            nn.Linear(in_features=64, out_features=64),
                                            nn.ReLU(),

                                            nn.Linear(in_features=64, out_features=32),
                                            nn.ReLU())

        # ==========================================================================================================================
        # Branch 2

        # Input FIR Coefficients to convolutional layers
        # Input shape = (Batch, Channel, Frequency Bins, Speakers, Taps)

        self.fir_cnn = nn.Sequential(nn.Conv3d(in_channels=1,
                                               out_channels=16,
                                               kernel_size=(3,3,5), # (Frequency, Speaker, Tap)
                                               padding=(1,1,2)),
                                     nn.ReLU(),

                                     nn.Conv3d(in_channels=16,
                                               out_channels=32,
                                               kernel_size=3,
                                               padding=1),
                                     nn.ReLU(),

                                     nn.MaxPool3d(kernel_size=2),

                                     nn.Conv3d(in_channels=32,
                                               out_channels=64,
                                               kernel_size=3,
                                               padding=1),
                                     nn.ReLU(),

                                     nn.AdaptiveAvgPool3d((1,1,1)))
        # ==========================================================================================================================
        # Branch 3

        # Input the Ideal Beam Pattern
        # Input shape = (Batch, Channel, Frequency Bins, Beam Angles)
        self.beam_cnn = nn.Sequential(nn.Conv2d(in_channels=1,
                                                out_channels=16,
                                                kernel_size=(3,3),
                                                padding=1),
                                      nn.ReLU(),

                                      nn.Conv2d(in_channels=16,
                                                out_channels=32,
                                                kernel_size=(3,3),
                                                padding=1),
                                      nn.ReLU(),

                                      nn.MaxPool2d(kernel_size=2),

                                      nn.Conv2d(in_channels=32,
                                                out_channels=64,
                                                kernel_size=(3,3),
                                                padding=1),
                                      nn.ReLU(),

                                      nn.AdaptiveAvgPool2d((1,1)))
        # ==========================================================================================================================
        # Feature Fusion Layer

        # Room features       = 32
        # Trajectory features = 32
        # FIR features        = 64
        # Beam features       = 64
        #
        # Total features      = 192

        self.fc = nn.Sequential(nn.Linear(in_features=192, out_features=256),
                                nn.ReLU(),

                                nn.Linear(in_features=256, out_features=512),
                                nn.ReLU(),

                                nn.Linear(in_features=512, out_features=512),
                                nn.ReLU())
        # ==========================================================================================================================
        # Output Layer

        # Output = FIR coefficient correction
        # Frequency bins x Speakers x Taps
        self.output = nn.Linear(in_features=512,
                                out_features=100*8*64)
    # ====================================================
    # Define Propagation Through Network
    def forward(self, room, trajectory, fir, beam):

        # ================================================
        # Extract features from each branch
        room_features = self.room_mlp(room)
        trajectory_features = self.trajectory_mlp(trajectory)

        # FIR Feature Extraction
        fir_features = self.fir_cnn(fir)
        fir_features = t.flatten(fir_features, start_dim=1)

        # Beam Pattern Feature Extraction
        beam_features = self.beam_cnn(beam)
        beam_features = t.flatten(beam_features, start_dim=1)

        # ================================================
        # Feature Fusion
        features = t.cat((room_features,
                          trajectory_features,
                          fir_features,
                          beam_features),
                          dim=1)

        # Fully Connected Feature Learning
        features = self.fc(features)

        # ================================================
        # FIR Correction Output

        correction = self.output(features)
        correction = correction.view(-1,100,8,64)

        # Residual Learning
        # Network learns correction to initial geometry-based FIR
        output = fir.squeeze(1) + correction

        return output
    # ========================================================