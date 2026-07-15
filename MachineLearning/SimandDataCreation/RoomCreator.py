# ========================================================
# RoomCreator.py
#
# Uses Pyroom Acoustics to create the room for simulation
# data. 
#
# Created by: Joseph Bozzo
# ========================================================
import pyroomacoustics as PA
import numpy as np
# ========================================================
class RoomCreator:
    # ====================================================
    # Create Room, Main Function to create rectangle room
    def create_room(self, L, W, H, Absorption, max_order):

        room_dim = [L,W,H]

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
    # Create Listener Position as receiver
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
    # Calculate Room Impulse Responses
    def get_rir(self, room): 
        return room.rir
    # ====================================================
    # Adds and positions a source within the room
    def add_source(self, room, position, source_signal):
        room.add_source(position, signal=source_signal)
    # ====================================================
    # Add and position receivers in the room (uses array)
    def add_receiver(self, room, mic_positions):
        mic_array = PA.MicrophoneArray(mic_positions, self.fs)
        room.add_microphone_array(mic_array)
        return mic_array
    # ====================================================
    # Runs the room simulation
    def simulate(self, room):
        room.simulate()
        return room.mic_array.signals
# ========================================================
        