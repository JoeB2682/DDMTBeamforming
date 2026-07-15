# ========================================================
#
# CreateTrainingData.py
#
# Main running script to use the training data creator 
# class to create and manage dataset.
#
# Created by: Joseph Bozzo
#
# ========================================================
# Imports
from TrainingDataCreator import TrainingDataCreator
# ========================================================
# Instanciate and create data samples
datacreator = TrainingDataCreator()

#for i in range(CREATE X AMOUNT OF SAMPLES):
#    datacreator.create_sample()

#datacreator.create_sample()

# Print Data sample to validate
datacreator.print_sample(0)
# ========================================================