# ========================================================
# CreateTrainingData.py
#
# Main running script to use the training data creator 
# class to create and manage dataset.
#
# Created by: Joseph Bozzo
# ========================================================
# Imports
import os
from TrainingDataCreator import TrainingDataCreator
from PlotSample import Sample_Plotter
# ========================================================
# Dataset Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
TRAINING_FOLDER = os.path.join(BASE_DIR, "TrainingData")
# ========================================================
# Constants / Params
SAMPLES_TO_CREATE = 0
SAMPLE_TO_PLOT = 0
PLOT_ALL = "NO"
# ========================================================
# Instanciate and create data samples
datacreator = TrainingDataCreator()
sampleplotter = Sample_Plotter()

samples = os.listdir(TRAINING_FOLDER)

for i in range(SAMPLES_TO_CREATE):
    datacreator.create_sample()

# Print or plot Data sample to validate
#datacreator.print_sample(0)

# plots one after another so doesn't break too horrifically
# (easier to kill)
if PLOT_ALL == "YES": 
    for sample_no in range(len(samples)):
        sampleplotter.plot_sample(sample_no)
else:
    if SAMPLE_TO_PLOT >= len(samples):
        pass
    if SAMPLE_TO_PLOT == -1:
        pass
    else:
        sampleplotter.plot_sample(SAMPLE_TO_PLOT)

datacreator.kill_all_data()
# ========================================================