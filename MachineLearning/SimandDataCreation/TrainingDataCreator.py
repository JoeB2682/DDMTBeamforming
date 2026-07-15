# ========================================================
#
# TrainingDataCreator.py
#
# Inherits other sub classes to neaten workflow.
#
# Created by: Joseph Bozzo
#
# ========================================================
# Module Imports
import os
import pyroomacoustics as PA
import numpy as np
import scipy.signal as signal

# Sub Classes
from RoomCreator import RoomCreator
from ArrayCreator import ArrayCreator
from SignalProcessor import SignalProcessor
from FeatureExtractor import FeatureExtractor
from DatasetManager import DatasetManager
from SampleCreator import SampleCreator
# ========================================================
# Constants
SAMPLERATE = 48000
FFTSIZE = 2**11

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")
# ========================================================
# Data Creation Class
class TrainingDataCreator(
        RoomCreator,
        ArrayCreator,
        SignalProcessor,
        FeatureExtractor,
        DatasetManager,
        SampleCreator
):
    # ====================================================
    # Constructor
    def __init__(self):

        self.fs = SAMPLERATE
        self.fft_size = FFTSIZE

        # Create training data folder if it does not exist
        os.makedirs(TRAINING_FOLDER, exist_ok=True)
 # ========================================================