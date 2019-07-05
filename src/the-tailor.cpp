#include "common.hpp"

struct TheTailor : Module {
	enum ParamId {
		GRAIN_SIZE_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		ENUMS(CH_INPUT, 2),
		PITCH_INPUT,
		SYNC_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		MAIN_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		ENUMS(CH_LIGHT, 2),
		NUM_LIGHTS
	};

	int counter1 = 0;
	int counter2 = 0;
	int playingNow = 0;
	float previousPitch = 0;

	dsp::SchmittTrigger schmittTrigger;

	TheTailor() {
		// Configure the module
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(GRAIN_SIZE_PARAM, 0.0f, 1.0f, 0.5f, "Cycle", " ratio");
	}

	void process(const ProcessArgs &args) override {

		if (schmittTrigger.process(rescale(inputs[SYNC_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f)))
		{
			float freq = dsp::FREQ_C4 * std::pow(2.f, inputs[PITCH_INPUT].getVoltage());
			freq = clamp(freq, 0.f, 20000.f);

			int samplesPerPeriod = (int)roundf((1.0f / freq) / args.sampleTime);

			counter1 = (int)roundf(params[GRAIN_SIZE_PARAM].getValue() * (float)samplesPerPeriod);
			counter2 = samplesPerPeriod - counter1;

			playingNow = 0;
		}

		if (counter1 <= 0)
		{
			playingNow = 1;
		}

		outputs[MAIN_OUTPUT].setVoltage(inputs[CH_INPUT + playingNow].getVoltage());

		lights[CH_LIGHT + 0].setBrightness(playingNow == 0 ? 1.0f : 0.0f);
		lights[CH_LIGHT + 1].setBrightness(playingNow == 1 ? 1.0f : 0.0f);

		counter1--;
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

		auto k = createParam<SLMSmallKnobGreen>(Vec(15, 82), module, TheTailor::GRAIN_SIZE_PARAM);
		k->snap = false;
		addParam(k);

		addInput(createInput<SLMInputPort>(Vec(17, 185), module, TheTailor::CH_INPUT + 0));
		addInput(createInput<SLMInputPort>(Vec(62, 185), module, TheTailor::CH_INPUT + 1));
		addInput(createInput<SLMInputPort>(Vec(62, 230), module, TheTailor::PITCH_INPUT));
		addInput(createInput<SLMInputPort>(Vec(17, 230), module, TheTailor::SYNC_INPUT));
		addOutput(createOutput<SLMOutputPort>(Vec(80, 327), module, TheTailor::MAIN_OUTPUT));

		addChild(createLight<MediumLight<RedLight>>(Vec(17, 210), module, TheTailor::CH_LIGHT + 0));
		addChild(createLight<MediumLight<RedLight>>(Vec(62, 210), module, TheTailor::CH_LIGHT + 1));
	}
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelTheTailor = createModel<TheTailor, TheTailorWidget>("the-tailor");
