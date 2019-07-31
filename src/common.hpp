#ifndef COMMON_H
#define COMMON_H

#include "plugin.hpp"

struct SLMTrimmer : app::SvgKnob {
	SLMTrimmer() {
		minAngle = -0.67*M_PI;
		maxAngle = 0.67*M_PI;
		//snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/trimmer.svg")));
	}
};

struct SLMSmallKnobGreen : app::SvgKnob {
	SLMSmallKnobGreen() {
		minAngle = -0.67*M_PI;
		maxAngle = 0.67*M_PI;
		//snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/knob-green.svg")));
	}
};

struct SLMBigKnobGreen : app::SvgKnob {
	SLMBigKnobGreen() {
		minAngle = -0.67*M_PI;
		maxAngle = 0.67*M_PI;
		//snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/knob-green-big.svg")));
	}
};

struct SLMLight : GrayModuleLightWidget {
	SLMLight(unsigned char r, unsigned char g, unsigned char b) {
		addBaseColor(nvgRGB(r, g, b));
	}
};

struct SLMRedLight : SLMLight {
	SLMRedLight() : SLMLight(0xFE, 0x71, 0x7D) {}
};

struct SLMSmallKnobRed : app::SvgKnob {
	SLMSmallKnobRed() {
		minAngle = -0.67*M_PI;
		maxAngle =  0.67*M_PI;
		//snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/knob-red.svg")));
	}
};

struct SLMBigKnobRed : app::SvgKnob {
	SLMBigKnobRed() {
		minAngle = -0.67*M_PI;
		maxAngle =  0.67*M_PI;
		//snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/knob-red-big.svg")));
	}
};

struct SLMInputPort : app::SvgPort {
	SLMInputPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/port.svg")));
	}
};

struct SLMOutputPort : app::SvgPort {
	SLMOutputPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/port.svg")));
	}
};

struct SLMToggleButton : app::SvgSwitch {
	SLMToggleButton() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button-off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button-on.svg")));
	}
};

struct SLMScrew : app::SvgScrew {
	SLMScrew() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/screw.svg")));
	}
};

#endif // COMMON_H
