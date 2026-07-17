# ========================================================
# TrajectoryCreator.py
#
# Creates trajectory for listener to simulate listener motion 
# within the acoustic simulations.  
#
# Created by: Joseph Bozzo
# ========================================================
import numpy as np
# ========================================================
class TrajectoryCreator:
    # ====================================================
    # Constructor
    def __init__(self, update_rate, duration):
          
          self.update_rate = update_rate
          self.duration = duration

          self.frames = int(duration * update_rate)

    # ====================================================
    # Creates random point path based on specified number of points
    def create_path(self, listener_start, no_points, L, W):

        # Has to assign first element before alocating it the start
        path = np.zeros((no_points + 1, 3))
        path[0] = listener_start

        for point in range(no_points):

            path[point + 1] = [
                np.random.uniform(1, L-1),
                np.random.uniform(1, W-1),
                1.5
            ]

        return path

    # ====================================================
    # Interpolates between generated points
    def create_trajectory(self, path):

        trajectory = []

        points_per_segment = int(
            self.frames / (len(path)-1)
        )

        for i in range(len(path)-1):

            segment = np.linspace(
                path[i],
                path[i+1],
                points_per_segment,
                endpoint=False
            )
            trajectory.extend(segment)

        trajectory.append(path[-1])

        return np.array(trajectory)
    # ====================================================
    # Resample trajectopry paths (so audio block and trajectory align)
    def resample_trajectory(self, trajectory, num_frames):

        current_frames = len(trajectory)

        # Already correct length
        if current_frames == num_frames:
            return trajectory

        old_time = np.linspace(0, 1, current_frames)
        new_time = np.linspace(0, 1, num_frames)

        resampled = np.zeros(
            (num_frames, 3)
        )

        # interpolate x,y,z separately
        for axis in range(3):
            resampled[:, axis] = np.interp(
                new_time,
                old_time,
                trajectory[:, axis]
            )

        return resampled
# ========================================================