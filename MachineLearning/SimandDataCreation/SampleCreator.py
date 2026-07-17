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
        # ================================================
        # Room and Geometry
        # ================================================

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
        # ================================================
        # Speaker Array
        # ================================================

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
        # ================================================
        # Listener Position + Movement
        # ================================================
        
        # Create listener starting position
        listener_start = self.create_listener_position(
            L,
            W,
            speaker_positions
        )

        # random number of points 
        no_points = np.random.randint(1,10)

        # Create random waypoints
        path = self.create_path(
            listener_start,
            no_points,
            L=L,
            W=W
        )

        # Create trajectory vectpr
        trajectory = self.create_trajectory(path)

        tau = []

        # Recaulculate TOA as listener moves
        for position in trajectory:
            frame_tau = self.calculate_TOA(
                speaker_positions,
                position
            )
            tau.append(frame_tau)

        tau = np.array(tau)

        # ================================================
        # Background Noise Position and Creation
        # ================================================

        # Create Position
        bgnoise_position = self.create_bgnoise_position(L, W)

        # Vary the level per data sample
        bgsignal_level = np.random.uniform(0.1, 1.0)

        bgnoise_signal, bgsigname = self.get_bg_noise_sample(bgsignal_level)
        bgsignal_data = "Filename: " + bgsigname

        # ================================================
        # Source Signal Creation
        # ================================================

        # For beam angle stuff
        est_narrow_freq = np.random.randint(200, 4000)

        # specifies which signal to use
        signal_type = np.random.randint(1, 6)
        signal_name = ""
        signal_data = ""

        if signal_type == 1:

            # souyrce = noise signal
            source_signal = self.create_noise_signal(self.fs)
            signal_name = "Noise"

        elif signal_type == 2:
            
            # random narrowband between 200 - 4000Hz
            freq = np.random.randint(200, 4000)
            est_narrow_freq = freq

            # Source = narrowband signal (pure tone)
            source_signal = self.create_narrowband(freq, self.fs)
            signal_name = "Narrowband"
            signal_data = f"Frequency: {freq}"

        elif signal_type == 3:
            
            # random wideband between 200 - 4000Hz
            f0 = np.random.randint(200, 1000)
            f1 = np.random.randint(f0, 2500)
            f2 = np.random.randint(f1, 4000)

            est_narrow_freq = f0

            # Source = random wideband chord
            source_signal = self.create_wideband_chord(f0, f1, f2, self.fs)
            signal_name = "Wideband"
            signal_data = f"Frequencies: {f0}, {f1}, {f2}"

        elif signal_type == 4:

            source_signal, filename = self.use_speech_signal()
            signal_name = "Speech"
            signal_data = "Filename: " + filename

        elif signal_type == 5:

            source_signal, filename = self.use_music_signal()
            signal_name = "Music"
            signal_data = "Filename: " + filename

        # ================================================
        # Split the Source Signal for motion tracking and apply delays
        # ================================================

        # split source signal recalclate beamformer each block 
        source_blocks = self.split_signal(source_signal)

        # To prevent trajectopry and block size mismatch resample
        num_frames = len(source_blocks)

        trajectory = self.resample_trajectory(
            trajectory,
            num_frames
        )

        # Store microphone positions for every frame
        mic_positions = trajectory.T
        # ================================================
        # Has tp recalculate TOA again
        tau = []

        for position in trajectory:
            frame_tau = self.calculate_TOA(
            speaker_positions,
            position)
            tau.append(frame_tau)

        tau = np.array(tau)
        # ================================================
        # create array for each speaker signal
        speaker_signals = [
            np.array([])
            for _ in range(num_speakers)
        ]

        # loop through blocks
        for frame, block in enumerate(source_blocks):
            
            # TOA for current frame 
            current_tau = tau[frame]
            max_tau = np.max(current_tau)

            for speaker in range(num_speakers):

                # calculate speaker delay 
                delay = max_tau - current_tau[speaker]

                # apply speaker delay
                delayed_block = self.apply_delay(
                    block,
                    delay
                )

                # concatenate blocks together 
                speaker_signals[speaker] = np.concatenate(
                (
                    speaker_signals[speaker],
                    delayed_block
                )
            )
                
        #print(len(source_blocks))
        #print(len(trajectory))

        # ================================================
        # Adding Speaker Delays and S/Rs to Room
        # ================================================

        # Iterate Speaker Positions
        for i, speaker in enumerate(speaker_positions.T):

            # Normalise
            speaker_signals[i] /= np.sqrt(num_speakers)

            # Add each speaker source to the room (beamformed source)
            self.add_source(
                room,
                position=speaker,
                source_signal=speaker_signals[i]
            )

        # Add receivers to room
        self.add_receiver(room, mic_positions)

        # Add Background Noise
        self.add_source(
            room,
            position=bgnoise_position,
            source_signal=bgnoise_signal
        )

        # ================================================
        # Run Room Simulation and get RIR/RT60
        # ================================================

        audio = self.simulate(room)

        # Fix audio length to 3 seconds 
        audio = self.fix_audio_length(audio, 3)
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

        # Get RT60 val for genereated room
        rt60 = self.get_RT60(room)

        # ================================================
        # Use Beam Pattern Generator for dir features
        # ================================================

        beam_patterns = []
        desired_dir = []

        # Recalculate beampatterns for every trajectory frame
        for position in trajectory:
            angles, response, listener_angle = self.generate_beam_pattern(
                speaker_positions,
                est_narrow_freq,
                position
            )
            beam_patterns.append(response)
            desired_dir.append(listener_angle)

        beam_pattern = np.array(beam_patterns)
        beam_angles = angles
        desired_dir = np.array(desired_dir)

        # Only plot the first frame 
        ideal_beamplot = self.plot_beam_pattern(beam_angles, beam_pattern[0], desired_dir[0])
        self.save_ideal_beamplot(ideal_beamplot)

        # ================================================
        # Create sample of data
        # ================================================

        # Dictionary to store sample
        room_data = {
            "room_dimensions": np.array( [L, W, H]),
            "absorption": Absorption,
            "rt60": rt60,
            "max_order": max_order,
            "array_type": array_type,
            "array_name": array_name,
            "num_speakers": num_speakers,
            "speaker_positions": speaker_positions,
            "Background_noise_position" : np.array(bgnoise_position),
            "listener_start_pos": np.array(listener_start),
            "trajectory" : trajectory,
            "Signal_Type" : signal_name,
            "Signal_Data" : signal_data,
            "Background_Noise" : bgsignal_data,
            "tau": tau,
            "mic_positions": mic_positions,
            "spectral_features": spectral_features,
            "rir": rir,
            "Beam_Patterns" : beam_pattern,
            "Beam_Angles" : beam_angles,
            "Desired_Angle" : desired_dir
        }

        # Save sample
        self.save_sample(audio, room_data)
# ========================================================
        