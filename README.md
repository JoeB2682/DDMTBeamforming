# Project Name

> DDMTBeamforming (Data-Driven-Motion-Tracked Beamforming).

---

## Overview

Joseph Bozzo MSc thesis 'Data-Driven Augmentation for Geometry Based Beamforming for Motion Tracked Audio'. For award MSc Audio and Music Technology at the University of York.

Abstract:

Obtaining the most optimal audible experience for listeners has been a significantly researched problem within audio and music technology posing a technical challenge across a variety of hardware setups and environments. It is especially challenging for motion tracked listeners due to constant change of the acoustic sweet spot and computational complexity of most methods which can effect accuracy and performance especially when implemented in real time systems.

Beamforming methods aim to achieve personal audio but often assume the best case scenario when calculating sound direction. Many techniques rely on imperfect estimation and adaptive mechanisms which can be computationally expensive especially in the context of motion tracking. Machine learning in current research has shown to be a promising technological paradigm across many fields significantly expanding the scope of what can achieved through the automation of tasks, increased speed of data processing and improving algorithm accuracy through better estimates.

This project presents a study on how existing geometric-based beamforming algorithms can be optimised through the use of neural-based techniques. Various beamforming approaches were investigated either through the evaluation of relevant literature or MATLAB simulations before being implemented in a C++ plugin titled Neural-Beamforming Optimiser.

The plugin demonstrates a novel optimisation pipeline which utilises a developed Convolutional- Neural-Network (BeamNet) trained on a created dataset of room scenario simulations. The Neural-Network model was developed using Pytorch and all dataset simulations were created using the Pyroomacoustics library.

Results highlighted the effectiveness of certain beamforming techniques in particular acoustic scenarios, how factors such as room geometry and speaker layout have significant impacts over the performance of beamforming algorithms and how even sparsely trained Neural-Networks can significantly mitigate performance diminishing factors. The presented pipeline provides a basis for future research and has many points at which it can be optimised and refined for the answering of greater research enquiries.

---

## Features

- MATLAB Simulations of DAS, FAS and MVDR beamforming algorithms using kwave

---

## Installation

### Requirements
- Python 3.x / MATLAB / JUCE / etc.
- Other dependencies

### Setup
```bash
git clone https://github.com/JoeB2682/DDMTBeamforming.git
cd DDMTBeamforming
pip install -r requirements.txt