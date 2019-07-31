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

## The Tailor
![Alt text](screenshots/the-tailor_panel.png?raw=true "The Tailor")

### short description
waveforms morphing tool, same concept as the [WTF Module](http://dove-audio.com/wtf-module/) from [Dove Audio](http://dove-audio.com/)

### long description
the tailor will cut a piece of waveform 2 (right side **IN** input) that will replace the corresponding piece from the waveform at left side **IN** input.

the center and the width of the piece to be cut is determined by the **CENTER** and **WIDTH** knobs (and correspoding CVs + attenuerters)

in order for the tailoring to be "musical", the module must operate in sync with one of the 2 waveforms period.

there are 2 methods of synchronization:
1) auto sync (**SYNC** input not connected) - the left side **in** waveform zero crossing rising edge determines the start of the cycle
2) external sync (**SYNC** input connected to a pulse or square waveform) - the start of the cycle is determined by the rising edge of the provided sync signal

In general, for simple waveforms (that is without multiple zero crossings within the cycle) auto sync will give best results, otherwise an external sync signal must be provided.

If ideally your VCO has a corresponding square wave out, you can use that as sync signal. 
However, depending on the actual VCO implementation, the square wave and your actual signal might not be perfectly in phase, especially at higher pitch. 

### additional considerations
#### buffer size
the module has an internal buffer of 100ksamples.

this means that for example, at 48kHz sampling rate, you would buffer overflow for a cycle slower than, roughly, 0.5Hz

when this happens, the red LED below the signal scope will light up to indicate this condition and the module won't produce any output.

This limits somehow the applicability of this module for slow LFOs morphing. However this is mainly intended to operate on VCOs although you can use it however you like, keeping in mind the said limitation.

#### aliasing
the "tailoring" is obviously a highly non-linear process and, depending on the actual waveforms in play, can easily introduce sudden discontinuities in your final signal which in digital domain translates into aliasing.

How much aliasing you can tollerate and methods to mitigate this problem are huge topics on their own. 

Future updates of this module might add optional miniBLEP around the window edges (for example) but at this stage it is behaving like the previusly mentioned eurorack module (that means, no special effort in minimizing aliasing)

### youtube demo
coming soon...

### donate
if you like my mmodules please consider to [donate](https://paypal.me/salvolm)
