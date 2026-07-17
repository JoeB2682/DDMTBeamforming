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
from BeamPatternGenerator import BeamPatternGenerator
# ========================================================
# Constants
SAMPLERATE = 48000
FFTSIZE = 2**11

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Training Data Directory
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")

# Beam Plot Directory 
PLOT_FOLDER = os.path.join(BASE_DIR, "IdealBeamPlots")
# ========================================================
# Data Creation Class
class TrainingDataCreator(
        RoomCreator,
        ArrayCreator,
        SignalProcessor,
        FeatureExtractor,
        DatasetManager,
        SampleCreator,
        BeamPatternGenerator
):
    # ====================================================
    # Constructor
    def __init__(self):

        self.fs = SAMPLERATE
        self.fft_size = FFTSIZE

        # Create training data folder if it does not exist
        os.makedirs(TRAINING_FOLDER, exist_ok=True)

    # ====================================================
    # Use if angry (deletes all samples in folder)
    def kill_all_data(self):
         
         # Iterates over each file in training data folder
         for file in os.listdir(TRAINING_FOLDER):

            # deletes everything with .npz extension
            if file.endswith(".npz"):

                # gets path of current file 
                filepath = os.path.join(TRAINING_FOLDER, file)

                # eradicates it 
                os.remove(filepath)

                # prints delete message
                print(f"Deleted: {file}")

         # Iterates over each file in plot data folder
         for file in os.listdir(PLOT_FOLDER):

            # deletes everything with .png extension
            if file.endswith(".png"):

                # gets path of current file 
                filepath = os.path.join(PLOT_FOLDER, file)

                # eradicates it 
                os.remove(filepath)

                # prints delete message
                print(f"Deleted: {file}")
 # ========================================================