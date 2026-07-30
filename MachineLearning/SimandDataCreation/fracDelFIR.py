# ========================================================
# fracDelFIR.py
#
# Python version of the fractional delay FIR filter used in 
# FAS beamformer implementation
#
# Created by: Joseph Bozzo
# ========================================================
import numpy as np
# ========================================================
class FilterGenerator:
    # ====================================================
    # Constructor
    def __init__(self, N, ArraySize, Fs):

        self.Fs = Fs
        self.N = N # no. taps
        self.ArraySize = ArraySize

        # Using float 32 aligns with C++ implementation
        self.b = np.zeros(self.N, dtype=np.float32)
        self.z = np.zeros(self.N, dtype=np.float32)
        self.win = np.zeros(self.N, dtype=np.float32)
        self.h = np.zeros(self.N, dtype=np.float32)

        # Set signal bandwidth...
        # Pre calculated based on spatial aliasing limit
        ARRAY_ALIAS_FREQ = 1900 

        self.fc = ARRAY_ALIAS_FREQ
        self.wc = 2.0 * np.pi * self.fc / self.Fs

        # Initialise Window
        self.win = self.generate_hanning(self.N)

        # Generate n (tap vector)
        self.n = np.arange(self.N, dtype=np.float32) - (self.N - 1) / 2.0

    # ====================================================
    # Hanning Window Generator
    def generate_hanning(self, windowLength):
        hanningWindow = 0.5 * (1 - np.cos((2 * np.pi * np.arange(windowLength))/(windowLength - 1)))
        return hanningWindow

    # ====================================================
    # Fractional Delay Amount (Interpolation)
    def calculateCurrentU(self, tau, tauMax):
        CurrentU = (tauMax - tau) * self.Fs
        return CurrentU

    # ====================================================
    # Seperately Generates Coefficients from audio process
    def generate_coefficients(self, tau, tauMax):

        u = self.calculateCurrentU(tau, tauMax)

        # Prevent division by zero
        if np.isclose(u % 1.0, 0.0):
            u += np.finfo(np.float32).eps

        # Generate h
        h = (np.sin(self.wc * (self.n - u)) /
            (np.pi * (self.n - u)))

        # Window
        b = h * self.win

        # Normalise
        b /= np.sum(b)

        # Store current filter coefficients
        self.b = b.astype(np.float32)

        # Copy and return
        return self.b.copy()
    
    # ====================================================
    # Main Filter Function
    def process(self, x):

        # Shift delay line
        self.z[1:] = self.z[:-1]

        # Insert current sample
        self.z[0] = x

        # FIR convolution
        y = np.dot(self.z, self.b)

        return y
        
    # ====================================================
    # Use Filter Function to Create Bank
    def createFilterBank(self, filterArray):
        pass
# ========================================================