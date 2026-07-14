# ========================================================
#
# CreateTrainingData.py
#
# Uses Pyroom Acoustics to create training data,
# Contains helper functions for pyroom acoustics objects.
#
# Follows workflow in C++ plugin to create random room 
# geometries and array configurations for training. 
#
# Created by: Joseph Bozzo
#
# ========================================================
# Module Imports
import os
import numpy as np
import scipy.signal as signal
import pyroomacoustics as PA
# ========================================================
# Constants
SAMPLERATE = 48000
FFTSIZE = 2**11
TRAINING_FOLDER = "TrainingData"
# ========================================================
# Data Creation Class
class TrainingDataCreator():
    # ====================================================
    # Constructor
    def __init__(self):

        self.fs = SAMPLERATE
        self.fft_size = FFTSIZE

        # Create training data folder if it does not exist
        os.makedirs(TRAINING_FOLDER, exist_ok=True)
    # ====================================================
    # Create Room
    def create_room(self, L, W, H, Absorption, max_order):
        room_dim = [L, W, H]
        room = PA.ShoeBox(
            room_dim,
            fs=self.fs,
            materials=PA.Material(
                energy_absorption=Absorption
            ),
            max_order=max_order
        )
        return room
    # ====================================================
    # Position Source
    def add_source(self, room, position, source_signal):
        room.add_source(position, signal=source_signal)
    # ====================================================
    # Position Receivers
    def add_receiver(self, room, mic_positions):
        mic_array = PA.MicrophoneArray(mic_positions, self.fs)
        room.add_microphone_array(mic_array)
        return mic_array
    # ====================================================
    # Create Listener Position
    def create_listener_position(self, L, W, speaker_positions):
        while True:
            position = np.array([
                np.random.uniform(1,L-1),
                np.random.uniform(1,W-1),
                1.5
            ])
            distance = np.linalg.norm(
                speaker_positions.T - position,
                axis=1
            )
            if np.min(distance) > 0.5:
                return position
    # ====================================================
    # Create Source Signal
    def create_signal(self, length):
        source_signal = np.random.randn(length)
        return source_signal
    # ====================================================
    # Apply Phase Delay to Source (like the beamformer) 
    def apply_delay(self, signal, delay):
        samples = int(delay * self.fs)
        delayed_signal = np.pad(signal, (samples, 0))
        delayed_signal = delayed_signal[:len(signal)]
        return delayed_signal
    # ====================================================
    # Simulate Room
    def simulate(self, room):
        room.simulate()
        return room.mic_array.signals
    # ====================================================
    # Calculate Room Impulse Responses
    def get_rir(self, room): 
        return room.rir
    # ====================================================
    # Create Speaker Positions (Linear Array)
    def create_linear_speaker_positions(self, num_speakers, spacing, height):
        speaker_positions = np.zeros((3, num_speakers))
        centre = (num_speakers - 1) / 2
        for i in range(num_speakers):
            speaker_positions[0, i] = ((i - centre) * spacing)
            speaker_positions[1, i] = 0
            speaker_positions[2, i] = height
        return speaker_positions
    # ====================================================
    # Create Circular Speaker Positions
    def create_cirular_speaker_array(self, num_speakers, radius, centre, height):
        speaker_positions = np.zeros((3, num_speakers))
        for i in range(num_speakers):
            angle = (2 * np.pi * i / num_speakers)
            speaker_positions[0, i] = (centre[0] + radius * np.cos(angle))
            speaker_positions[1, i] = (centre[1] + radius * np.sin(angle))
            speaker_positions[2, i] = height
        return speaker_positions
    # ====================================================
    # Calculate Time-Of-Arrival (from speakers to listener)
    def calculate_TOA(self, speaker_positions, listener_position):
        # Pretty much the same as MATLAB and C++ 
        c = 343  # speed of sound (m/s)
        tau = []

        for speaker in speaker_positions.T:
            distance = np.linalg.norm(
                speaker - listener_position
            )
            delay = distance / c
            tau.append(delay)

        return np.array(tau)
    # ====================================================
    # Extract Spectral Features
    def extract_spectral_features(self, audio):
        frequencies, times, spectrum = signal.stft(
            audio[0],
            fs=self.fs,
            nperseg=self.fft_size,
            noverlap=self.fft_size // 2
        )
        magnitude = np.abs(spectrum)
        return magnitude
    # ====================================================
    # Fix Audio Length (ensures generated audio is the same length),
    # fixes using sample rate specified
    def fix_audio_length(self, audio):
        target_length = self.fs
        if audio.shape[1] > target_length:
            audio = audio[:, :target_length]
        else:
            padding = target_length - audio.shape[1]
            audio = np.pad(audio, ((0,0), (0,padding)))
        return audio
    # ====================================================
    # Get Next Sample Number (for automatic indexing in folder)
    def get_next_sample_number(self):
        # locate folder
        files = os.listdir(TRAINING_FOLDER)

        # get blank array of sample numbers
        sample_numbers = []

        # iterate files
        for file in files:
            # find last file and increment with naming convention
            if file.startswith("sample_") and file.endswith(".npz"):
                number = file.replace("sample_", "")
                number = number.replace(".npz", "")
                sample_numbers.append(int(number))

        if len(sample_numbers) == 0:
            return 0

        return max(sample_numbers) + 1
    # ====================================================
    # Save Data Sample (uses os to index and name properly)
    def save_sample(self, audio, room_data):

        # use prior defined function to get next sample
        sample_number = self.get_next_sample_number()

        # create filename
        filename = f"sample_{sample_number:05d}.npz"

        # specify file path
        filepath = os.path.join(TRAINING_FOLDER, filename)

        np.savez(filepath, audio=audio, **room_data)

        print("Saved:", filename)

    # ====================================================
    # Print Data Sample (pretty looking data printing)
    def print_sample(self, sample_no):

        # Specify sample name using input variable
        filename = f"sample_{sample_no:05d}.npz"

        data = np.load(f"TrainingData/{filename}", allow_pickle=True)

        print("\n================================")
        print(f"        {filename}")
        print("================================\n")

        # Print each data entry
        for key in data.files:

            value = data[key]

            print(f"{key}")
            print("--------------------------------")

            if np.isscalar(value) or value.ndim == 0:
                print(f"Value: {value.item()}")

            else:

                print(f"Shape: {value.shape}")

                if value.size <= 20:
                    print("Value:")
                    print(value)

            print()
        print("================================\n")
    # ====================================================
    # Main running function to create data sample
    def create_sample(self):

        # Create random room parameters
        L = np.random.uniform(4, 8)
        W = np.random.uniform(4, 8)
        H = np.random.uniform(2.5, 3.5)

        Absorption = np.random.uniform(0.2, 0.8)
        max_order = np.random.randint(5, 20)

        # Create room
        room = self.create_room(
            L=L,
            W=W,
            H=H,
            Absorption=Absorption,
            max_order=max_order
        )

        # Create random speaker array
        array_type = np.random.randint(0, 2) # Randomly chooses between cicular and linear array
        num_speakers = np.random.randint(8, 32)
        array_height = np.random.uniform(1, H - 0.5)
        array_name = ""

        if array_type == 0:

            # Linear array

            array_name = "linear"

            spacing = np.random.uniform(0.05, 0.2)

            # Ensure array fits inside room
            max_speakers = int((L - 1) / spacing)
            num_speakers = np.random.randint(8, min(32, max_speakers))
            array_width = (num_speakers - 1) * spacing

            centre_x = np.random.uniform(
                array_width / 2 + 0.5,
                L - array_width / 2 - 0.5
            )

            centre_y = np.random.uniform(0.5, W - 0.5)

            speaker_positions = self.create_linear_speaker_positions(
                num_speakers,
                spacing,
                array_height
            )

            speaker_positions[0, :] += centre_x
            speaker_positions[1, :] += centre_y

        else:

            # Circular array

            array_name = "circular"

            radius = np.random.uniform(0.5, min(L, W) / 3)
            centre_x = np.random.uniform(radius, L - radius)
            centre_y = np.random.uniform(radius, W - radius)

            speaker_positions = self.create_cirular_speaker_array(
                num_speakers,
                radius,
                [centre_x, centre_y],
                array_height
            )

        # Create listener position
        listener_position = self.create_listener_position(L, W, speaker_positions)

        # Create mic pos
        mic_positions = np.array(listener_position).reshape(3,1)

        # Calculate Time Of Arrival (tau)
        tau = self.calculate_TOA(
            speaker_positions,
            np.array(listener_position)
        )

        # Create source signal
        source_signal = self.create_signal(self.fs)

        # Add delayed speaker signals
        for i, speaker in enumerate(speaker_positions.T):
            delayed_signal = self.apply_delay(
                source_signal,
                tau[i]
            )
            # normalise
            delayed_signal /= np.sqrt(num_speakers)
            
            self.add_source(
                room,
                position=speaker,
                source_signal=delayed_signal
            )

        # Add receivers to room
        self.add_receiver(
            room,
            mic_positions
        )

        # Run simulation
        audio = self.simulate(room)

        # Fix audio length to 1 second
        audio = self.fix_audio_length(audio)
        print("Audio shape:", audio.shape)

        # Extract Spectral Features
        spectral_features = self.extract_spectral_features(audio)

        # Get room impulse responses
        rir = self.get_rir(room)

        # Convert or it has a fit
        rir = np.array(
            self.get_rir(room),
            dtype=object
        )

        # Create sample of data
        room_data = {
            "room_dimensions": np.array( [L, W, H]),
            "absorption": Absorption,
            "max_order": max_order,
            "array_type": array_type,
            "array_name": array_name,
            "num_speakers": num_speakers,
            "speaker_positions": speaker_positions,
            "listener_position": np.array(listener_position),
            "tau": tau,
            "mic_positions": mic_positions,
            "spectral_features": spectral_features,
            "rir": rir
        }

        # Save sample
        self.save_sample(audio, room_data)

# ========================================================
# Instanciate and create data samples
datacreator = TrainingDataCreator()

#for i in range(CREATE X AMOUNT OF SAMPLES):
#    datacreator.create_sample()

#datacreator.create_sample()

# Print Data sample to validate
datacreator.print_sample(7)
# ========================================================