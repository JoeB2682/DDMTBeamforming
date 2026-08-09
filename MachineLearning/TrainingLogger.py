# ========================================================
# 
# TrainingLogger.py
#
# Takes the data (loss, current epoch etc...) and saves to 
# .csv for graphing purposes in the report
#
# Created by: Joseph Bozzo
#
# ========================================================
# Imports
import os
import csv
# ========================================================
# Directories

# Base Project Directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# .csv File Directory
TRAINING_LOG = os.path.join(BASE_DIR, "TrainingLog")

# ========================================================
# Training Logger Class
class TrainingLogger:
    # ====================================================
    # Constructor
    def __init__(self, train_attempt, learning_rate):

        # Create CSV filename
        self.filename = f"Train_Attempt_{train_attempt:05d}.csv"
        self.filepath = os.path.join(TRAINING_LOG, self.filename)

        # Open CSV file
        self.file = open(self.filepath, "w", newline="")

        # Create CSV writer
        self.writer = csv.writer(self.file)

        # Header
        self.writer.writerow(["Epoch", "Current Loss", "Average Loss", "Learning Rate"])
    # ====================================================
    # Write current line of data
    def Writelineofdata(self, Epoch, CurrentLoss, Average_Loss, Learning_Rate):

        # Write current row
        self.writer.writerow([Epoch, CurrentLoss, Average_Loss, Learning_Rate])

        # Make sure data is written immediately
        self.file.flush()
    # ====================================================
    # Close the file
    def close(self):
        self.file.close()
# ========================================================