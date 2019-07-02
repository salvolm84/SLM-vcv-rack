#ifndef COMMON_H
#define COMMON_H

#include "plugin.hpp"

struct SLMSmallKnobGreen : app::SvgKnob {
	SLMSmallKnobGreen() {
		minAngle = -0.67*M_PI;
		maxAngle = 0.67*M_PI;
		snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/small_knob_green.svg")));
	}
};

struct SLMSmallKnobPink : app::SvgKnob {
	SLMSmallKnobPink() {
		minAngle = -0.67*M_PI;
		maxAngle =  0.67*M_PI;
		snap = true;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/small_knob_pink.svg")));
	}
};

struct SLMInputPort : app::SvgPort {
	SLMInputPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/input.svg")));
	}
};

struct SLMOutputPort : app::SvgPort {
	SLMOutputPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/output.svg")));
	}
};

struct SLMToggleButton : app::SvgSwitch {
	SLMToggleButton() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_on.svg")));
	}
};

struct SLMScrew : app::SvgScrew {
	SLMScrew() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/screw.svg")));
	}
};

#endif // COMMON_H
