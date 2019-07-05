#include "common.hpp"

struct TheTailor : Module {
	enum ParamId {
		GRAIN_SIZE_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		CH1_INPUT,
		CH2_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		MAIN_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		CH1_LIGHT,
		CH2_LIGHT,
		NUM_LIGHTS
	};

	TheTailor() {
		// Configure the module
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(DIVISION_PARAM, 2.0f, 64.0f, 2.0f, "Division Factor", " counts");
	}

	void process(const ProcessArgs &args) override {

		bool trigger = schmittTrigger.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f));
		int currentDivision = (int)params[DIVISION_PARAM].getValue();

		if (currentDivision != previousDivision)
		{
			counter = currentDivision - 2;
			previousDivision = currentDivision;
		}

		if (trigger)
		{
			inLightStatus = !inLightStatus;

			if (counter == 0)
			{
				counter = currentDivision - 2;
				outStatus = !outStatus;
			}
			else
			{
				counter--;
			}
			
		}

		outputs[CLOCK_OUTPUT].setVoltage(outStatus ? 10.0f : 0.0f);
		lights[IN_LIGHT].setBrightness(inputs[CLOCK_INPUT].getVoltage() < 0.5f ? 1.f : 0.f);
		lights[OUT_LIGHT].setBrightness(outputs[CLOCK_OUTPUT].getVoltage() < 0.5f ? 1.f : 0.f);

	}
};


struct TheTailorWidget : ModuleWidget {
	TheTailorWidget(TheTailor *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/mux-freak.svg")));

		addChild(createWidget<SLMScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<SLMScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<SLMScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<SLMScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInput<SLMInputPort>(Vec(25, 328), module, Clocky::CLOCK_INPUT));
		addOutput(createOutput<SLMOutputPort>(Vec(80, 327), module, Clocky::CLOCK_OUTPUT));

		addParam(createParam<SLMSmallKnobPink>(Vec(15, 132), module, Clocky::DIVISION_PARAM));

		addChild(createLight<MediumLight<RedLight>>(Vec(41, 59), module, Clocky::IN_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(Vec(41, 79), module, Clocky::OUT_LIGHT));
	}
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelTheTailor = createModel<TheTailor, TheTailorWidget>("the-tailor");
