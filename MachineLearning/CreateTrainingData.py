# ========================================================
# 
# CreateTrainingData.py
#   
# Uses Pyroom Acoustics to create training data
#
# Created by: Joseph Bozzo
#    
# ========================================================
# Module Imports
import torch as t
from torch import nn
from torch.utils.data import DataLoader
from torchvision.transforms import ToTensor
from torchvision import datasets

import pyroomacoustics as PA
# ========================================================
# Constants 
SAMPLERATE = 48000
FFTSIZE = 2^11
# ========================================================
# Data Creation Class
class TrainingDataCreator():
    # ====================================================
    # Constructor
    def __init__(self):

        pass
    # ====================================================
    # Define Propogation Through Network
    def main(self):
        print("Training Main")

# ========================================================
# Instanciate and run main for testing and creation
datacreator = TrainingDataCreator()
datacreator.main()
# ========================================================