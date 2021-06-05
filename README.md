## Subsynth - A Digital Synthesizer Project

### Authors: 
Andrew Wilson (anwils2@pdx.edu) <br> 
Robin Su (robisu@pdx.edu) <br>
Aaron Hudson (ahuds2@pdx.edu) <br>

---
### CS 510: Computers, Sound and Music

Professor Bart Massey <br>
Spring 2021 <br>
Portland State University

---
### Project Description

This project contains code to generate a subtractive synthesizer plug-in, built in C++ using the JUCE application framework.  The synthesizer is polyphonic, allowing six voices to play simultaneously across 10 octaves, and has a graphical user interface containing the following components:

- Interactive On-screen MIDI keyboard
  - receives input from mouse or keyboard
- Waveform Selector
  - choice of four waveform oscillators: sine, square, saw, or triangle
- ADSR Envelope
  - allows the user to set the attack, decay, sustain, and release (range 0.0-1.0 for each)
- Variable State Filter
  - choice of three filter types: low-pass, band-pass, or high-pass
  - allows the user to set the resonance (range 1.0-5.0) and cutoff frequency (range 0-20,000 Hz)
- Waveform Visualizer
  - displays a visual depiction of waveforms being generated


### Basic Project Goals
 - Implement basic digital sound processing techniques
 - Build a basic synthesizer with the following properties:
   - monophonic
   - singular waveform oscillator (sine or square)
   - variable low-pass filter
   - ADSR envelope
   - Barebones UI
   - Basic input for 7 main musical notes (A, B, C, D, E, F, and G)
 - Learn the JUCE framework

### Stretch Goals
- Add the following features to the synthesizer:
  - polyphonic
  - multiple waveform oscillators (sine, square, saw, triangle)
  - low-pass filter with resonance parameter
  - low frequency oscillator attached to filter cutoff
  - MIDI input
  - full 12 note octave support

### What We Achieved

We were able to accomplish all of our initial goals outlined above, along with implementing many of the stretch goals we identified.  Furthermore, we expanded upon many of these stretch goals to deliver a final product allowing much greater customization of the sound being generated.

### How It Went

The JUCE framework had a large barrier of entry to overcome, as it provides a very large and complex toolkit that requires a high level of understanding to utilize.  Thankfully, the documentation for JUCE is very thorough, and there were many helpful tutorials to access when learning about different capabilities.  Furthermore, we had weekly meetings to go over what each of us accomplished over the prior week, and these sessions often consisted of group programming to help debug any issues blocking progress forward.  Overall, this project was challenging due to us being rusty with C++, while also learning about digital audio processing concepts as we went, but our group was very good about helping each other out as necessary and problem solving issues together.

### How We Tested

Our testing plan consisted of two main methods:
- Unit Testing:
  
  We wrote a testing suite to verify the basic components of our plug-in functioned as expected.  Specifically, we focused on testing that changes to the oscillator, ADSR envelope, and filter returned the expected results.
  
- Output Comparison Testing:
  
  We also analyzed the output of our plug-in using visualization tools and compared the results against outputs known to perform similar effects to ensure our synthesizer was modifying the generated sound as expected. 
  
***add images of test outputs to this section?***

### Additional sections/questions to cover:
What worked?  

What didn't?  

How satisfied are you with the result?  

What would you like to improve in the future?

---
## Setup

- Install JUCE
- Clone repo
- Open the `.jucer` file with the Projucer
- Export files and launch IDE from inside Projucer - compatible with Visual Studio (Windows), XCode (MacOS) and Linux
- Build (if building on a Mac, see below for further details)

## Run

### Windows
- Navigate to `./Builds/YOUR EDITOR/x64/Debug/Standalone Plugin/`
- Launch `Subsynth` executable (or w/e it is on MacOS)

### MacOS
 - After exporting to XCode, be sure to select `Standalone Plugin` as the active scheme before building (the Projucer configures each project to build different target application types. For this project, we built a standalone plugin). 
 - If building the application does not automatically launch the plugin, navigate to `./Builds/MacOSX/build/Debug/` and double-click on `Subsynth`.\

### References
***fill in actual reference information later***

JUCE

Audio Programmer Youtube Channel

CS510 course materials
