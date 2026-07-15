# ========================================================
# SampleCreator.py
#
# Creates data samples.
#
# Randomises Pyroom and various other parameters to create 
# varied scenarios across data samples
#
# Created by: Joseph Bozzo
# ========================================================
import numpy as np
# ========================================================
class SampleCreator:
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

         # Create source signal (Noise)
        source_signal = self.create_noise_signal(self.fs)

        # Add delayed speaker signals
        max_tau = np.max(tau)

        # Iterate Speaker Positions
        for i, speaker in enumerate(speaker_positions.T):

            # Apply beamformer steering delay (relative delay)
            beam_delay = max_tau - tau[i]

            delayed_signal = self.apply_delay(
                source_signal,
                beam_delay
            )

            # normalise
            delayed_signal /= np.sqrt(num_speakers)
            
            # Add sources to room
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
        