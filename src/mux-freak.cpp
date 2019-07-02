#include "common.hpp"
#include <ctime>

struct MuxFreak : Module {
	enum ParamId {
		ENUMS(LEN_PARAM, 4),
		ENUMS(PROB_PARAM, 4),
		ENUMS(SOLO_PARAM, 4),
		NUM_PARAMS
	};
	enum InputId {
		CLK_INPUT,
		ENUMS(CH_INPUT, 4),
		ENUMS(LEN_INPUT, 4),
		ENUMS(PROB_INPUT, 4),
		NUM_INPUTS
	};
	enum OutputId {
		TRIG_OUTPUT,
		MUX_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};

	float relativeProb[4];
	int playingNow = 0;
	int remainingTicks = 1;
	float clockBefore = 0.0f;
	int soloIndex = -1;

	dsp::PulseGenerator pulseGenerator;
	dsp::BooleanTrigger buttonTrigger[4];

	MuxFreak() {
		// Configure the module
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		// Configure parameters
		// See engine/Param.hpp for config() arguments
		// void configParam(int paramId, float minValue, float maxValue, float defaultValue, std::string label = "", std::string unit = "", float displayBase = 0.f, float displayMultiplier = 1.f, float displayOffset = 0.f)
		configParam(SOLO_PARAM + 0, 0.0f, 1.0f, 0.0f, "CH1 Solo");
		configParam(SOLO_PARAM + 1, 0.0f, 1.0f, 0.0f, "CH2 Solo");
		configParam(SOLO_PARAM + 2, 0.0f, 1.0f, 0.0f, "CH3 Solo");
		configParam(SOLO_PARAM + 3, 0.0f, 1.0f, 0.0f, "CH4 Solo");
		
		configParam(LEN_PARAM + 0, 2.0f, 64.0f, 8.0f, "CH1 Length", " clock counts");
		configParam(LEN_PARAM + 1, 2.0f, 64.0f, 8.0f, "CH2 Length", " clock counts");
		configParam(LEN_PARAM + 2, 2.0f, 64.0f, 8.0f, "CH3 Length", " clock counts");
		configParam(LEN_PARAM + 3, 2.0f, 64.0f, 8.0f, "CH4 Length", " clock counts");

		configParam(PROB_PARAM + 0, 0.0f, 100.0f, 25.0f, "CH1 Probability", "%");
		configParam(PROB_PARAM + 1, 0.0f, 100.0f, 25.0f, "CH2 Probability", "%");
		configParam(PROB_PARAM + 2, 0.0f, 100.0f, 25.0f, "CH3 Probability", "%");
		configParam(PROB_PARAM + 3, 0.0f, 100.0f, 25.0f, "CH4 Probability", "%");

		srand (static_cast <unsigned> (time(0)));
	}

	int coinToss()
	{
		if (soloIndex >= 0)
		{
			return soloIndex;
		}

		// calculate the relative probabilities by filtering out not active channels
		float probSum = 0;
		for (int i = 0; i < 4; i++)
		{
			if (inputs[CH_INPUT + i].isConnected())
				probSum += 0.01f * clamp(params[PROB_PARAM + i].getValue() + inputs[PROB_INPUT + i].getVoltage(), 0.0f, 100.0f);
		}

		for (int i = 0; i < 4; i++)
		{
			if (inputs[CH_INPUT + i].isConnected())
			{
				relativeProb[i] = 0.01f * clamp(params[PROB_PARAM + i].getValue() + inputs[PROB_INPUT + i].getVoltage(), 0.0f, 100.0f) / probSum;
			}
			else
			{
				relativeProb[i] = 0.0f;
			}
				
		}

		// coin toss
		float coin = (float)rand() / RAND_MAX;

		float p0 = relativeProb[0];
		float p1 = p0 + relativeProb[1];
		float p2 = p1 + relativeProb[2];

		if (coin <= p0)
		{
			return 0;
		}

		if (coin > p0 && coin <= p1)
		{
			return 1;
		}

		if (coin > p1 && coin <= p2)
		{
			return 2;
		}

		return 3;
	}

	void process(const ProcessArgs &args) override {

		outputs[TRIG_OUTPUT].setVoltage(pulseGenerator.process(args.sampleTime) ? 10.0f : 0.0f);

		// check solo buttons
		for (int i = 0; i < 4; i++)
		{
			bool triggered = buttonTrigger[i].process(params[SOLO_PARAM + i].getValue() > 0.0f);
			if (triggered)
			{
				for (int j = 0; j < 4; j++)
				{
					if (j != i)
					{
						params[SOLO_PARAM + j].setValue(0.0f);
					}
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (params[SOLO_PARAM + i].getValue() > 0.0f)
			{
				soloIndex = i;
				break;
			}
			soloIndex = -1;
		}

		// no clock or no active inputs -> nothing to do!
		if (!inputs[CLK_INPUT].isConnected() || (!inputs[CH_INPUT + 0].isConnected() && 
		!inputs[CH_INPUT + 1].isConnected() && 
		!inputs[CH_INPUT + 2].isConnected() &&
		!inputs[CH_INPUT + 3].isConnected()))
		{
			return;
		}

		// detect clock rising edge
		float clockNow = inputs[CLK_INPUT].getVoltage();
		
		if (clockNow - clockBefore > 0.5f)
		{
			remainingTicks--;
			if (remainingTicks == 0) // time for a new coin toss!
			{
				playingNow = coinToss();
				remainingTicks = (int)round(clamp(params[LEN_PARAM + playingNow].getValue() + inputs[LEN_INPUT + playingNow].getVoltage(), 2.0f, 64.0f));
				pulseGenerator.trigger();
			}
		}
		
		clockBefore = clockNow;
		outputs[MUX_OUTPUT].setVoltage(inputs[CH_INPUT + playingNow].getVoltage());
		
	}
};


struct MuxFreakWidget : ModuleWidget {
	MuxFreakWidget(MuxFreak *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/mux-freak.svg")));

		addChild(createWidget<SLMScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<SLMScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<SLMScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<SLMScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<SLMToggleButton>(Vec(20, 35), module, MuxFreak::SOLO_PARAM + 0));
		addParam(createParam<SLMToggleButton>(Vec(65, 35), module, MuxFreak::SOLO_PARAM + 1));
		addParam(createParam<SLMToggleButton>(Vec(110, 35), module, MuxFreak::SOLO_PARAM + 2));
		addParam(createParam<SLMToggleButton>(Vec(155, 35), module, MuxFreak::SOLO_PARAM + 3));

		addParam(createParam<SLMSmallKnobGreen>(Vec(15, 82), module, MuxFreak::PROB_PARAM + 0));
		addParam(createParam<SLMSmallKnobGreen>(Vec(60, 82), module, MuxFreak::PROB_PARAM + 1));
		addParam(createParam<SLMSmallKnobGreen>(Vec(105, 82), module, MuxFreak::PROB_PARAM + 2));
		addParam(createParam<SLMSmallKnobGreen>(Vec(150, 82), module, MuxFreak::PROB_PARAM + 3));

		addParam(createParam<SLMSmallKnobPink>(Vec(15, 132), module, MuxFreak::LEN_PARAM + 0));
		addParam(createParam<SLMSmallKnobPink>(Vec(60, 132), module, MuxFreak::LEN_PARAM + 1));
		addParam(createParam<SLMSmallKnobPink>(Vec(105, 132), module, MuxFreak::LEN_PARAM + 2));
		addParam(createParam<SLMSmallKnobPink>(Vec(150, 132), module, MuxFreak::LEN_PARAM + 3));

		addInput(createInput<SLMInputPort>(Vec(17, 185), module, MuxFreak::CH_INPUT + 0));
		addInput(createInput<SLMInputPort>(Vec(62, 185), module, MuxFreak::CH_INPUT + 1));
		addInput(createInput<SLMInputPort>(Vec(107, 185), module, MuxFreak::CH_INPUT + 2));
		addInput(createInput<SLMInputPort>(Vec(152, 185), module, MuxFreak::CH_INPUT + 3));

		addInput(createInput<SLMInputPort>(Vec(17, 230), module, MuxFreak::PROB_INPUT + 0));
		addInput(createInput<SLMInputPort>(Vec(62, 230), module, MuxFreak::PROB_INPUT + 1));
		addInput(createInput<SLMInputPort>(Vec(107, 230), module, MuxFreak::PROB_INPUT + 2));
		addInput(createInput<SLMInputPort>(Vec(152, 230), module, MuxFreak::PROB_INPUT + 3));

		addInput(createInput<SLMInputPort>(Vec(17, 275), module, MuxFreak::LEN_INPUT + 0));
		addInput(createInput<SLMInputPort>(Vec(62, 275), module, MuxFreak::LEN_INPUT + 1));
		addInput(createInput<SLMInputPort>(Vec(107, 275), module, MuxFreak::LEN_INPUT + 2));
		addInput(createInput<SLMInputPort>(Vec(152, 275), module, MuxFreak::LEN_INPUT + 3));

		addInput(createInput<SLMInputPort>(Vec(25, 328), module, MuxFreak::CLK_INPUT));

		addOutput(createOutput<SLMOutputPort>(Vec(80, 327), module, MuxFreak::TRIG_OUTPUT));
		addOutput(createOutput<SLMOutputPort>(Vec(129, 327), module, MuxFreak::MUX_OUTPUT));
	}
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelMuxFreak = createModel<MuxFreak, MuxFreakWidget>("mux-freak");
