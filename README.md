# SLM-vcv-rack
plugins for [VCV Rack virtual modular synthesizer](https://vcvrack.com/)

## Mux Freak

![Alt text](screenshots/mux-freak_panel.png?raw=true "Mux Freak")

### short description
clocked 4 to 1 mux with random switching, probability and length controls.

### long description
Each of the 4 channels has a **PROBABILITY** and **LENGTH** control.
The **PROBABILITY** (0% to 100%) determines how likely that channel is going to be selected whenever a random switch will occour
The **LENGTH** (2 to 64 clock cycles) determines how long that channel will play once it had been selected.

After the selected channel has been played for the **LENGTH** number of cycles, a new random mux switch will take place.
On each mux switching a pulse is generated on the **TRIG** output. This can be used to trigger other modules.

To momentarily bypass the mux random switching, a **SOLO** button is available for each channel. This can be handy to audition the channel while racking or in some live situations.

note that the switching will always happens on multiple of the input clock. So it is possibile to sync with master clock wherever desired.

**CVs** to modulate **PROBABILITY** and **LENGTH** are available.

### youtube demo
coming soon...
