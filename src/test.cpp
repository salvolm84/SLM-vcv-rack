#include "common.hpp"

struct WaveformPiece {
    float buffer[10 * 1024] = {0.0f};
    int size;
    float strokeWidth;
    NVGcolor strokeColor;
};

struct Test : Module {
	enum ParamId {
        WINDOW_CENTER_PARAM,
        WINDOW_WIDTH_PARAM,
        WINDOW_CENTER_ATTENUERTER,
        WINDOW_WIDTH_ATTENUERTER,
		NUM_PARAMS
	};
	enum InputId {
        CH1_INPUT,
        CH2_INPUT,
        SYNC_INPUT,
        WINDOW_CENTER_CV,
        WINDOW_WIDTH_CV,
		NUM_INPUTS
	};
	enum OutputId {
        MAIN_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};

    int sampleCounter = 0;
    int samplesPerPeriod = 0;
    int syncCount = 0;

    //struct WaveformPiece waveform[3];

    dsp::SchmittTrigger schmittTrigger;
    bool readyToDisplay = false;
    
    float displayBuffer[20 * 1024] = {0.0f};

	Test() {
		// Configure the module
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(WINDOW_CENTER_PARAM, 0.0f, 1.0f, 0.5f, "Window Center");
        configParam(WINDOW_WIDTH_PARAM, 0.0f, 1.0f, 0.3f, "Window Width");
	}

	void process(const ProcessArgs &args) override {

        if (!inputs[SYNC_INPUT].isConnected())
            return;

        if (schmittTrigger.process(rescale(inputs[SYNC_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f))) {
            samplesPerPeriod = sampleCounter;
            sampleCounter = 0;
            
            readyToDisplay = true;
        }

        float samplePosition = (float)sampleCounter / (float)samplesPerPeriod;
        
        float windowCenterCv = inputs[WINDOW_CENTER_CV].getVoltage() * params[WINDOW_CENTER_ATTENUERTER].getValue();
        float windowWidthCv = inputs[WINDOW_WIDTH_CV].getVoltage() * params[WINDOW_WIDTH_ATTENUERTER].getValue();
        
        float windowCenter = params[WINDOW_CENTER_PARAM].getValue() + windowCenterCv;
        float windowWidth = params[WINDOW_WIDTH_PARAM].getValue() + windowWidthCv;

        windowCenter = clamp(windowCenter, 0.0f, 1.0f);
        windowWidth = clamp(windowWidth, 0.0f, 1.0f);
 
        if (samplePosition >= (windowCenter - windowWidth / 2.0f) && samplePosition <= (windowCenter + windowWidth / 2.0f)) {
            outputs[MAIN_OUTPUT].setVoltage(inputs[CH2_INPUT].getVoltage());
        } else {
            outputs[MAIN_OUTPUT].setVoltage(inputs[CH1_INPUT].getVoltage());
        }

        displayBuffer[sampleCounter++] = outputs[MAIN_OUTPUT].getVoltage();

	}
};

struct TestDisplay : TransparentWidget {
	Test *module;

    void draw(const DrawArgs &args) override {
        if (!module)
			return;

        if (!module->readyToDisplay)
            return;

        int size = module->samplesPerPeriod;
        
        float width = box.size.x;
        float deltaTime = width / (float)size;


        float max = -10000000.0;
        float min = +10000000.0;

        for (int i = 0; i < size; i++)
        {
            if (module->displayBuffer[i] > max)
            {
                max = module->displayBuffer[i];
            }

            if (module->displayBuffer[i] < min)
            {
                min = module->displayBuffer[i];
            }
        }

        // float drawHeight = 100.0f;
        float center = box.size.y / 2.0f;
        float gain = - 8.0f;
        
        nvgSave(args.vg);
        nvgBeginPath(args.vg);

        nvgMoveTo(args.vg, 0, center + gain * module->displayBuffer[0]);
        
        for (int i = 0; i < size; i++) {
            nvgLineTo(args.vg, (float)i * deltaTime, center + gain * module->displayBuffer[i]);
        }

        nvgStrokeWidth(args.vg, 1.0f);
        nvgStrokeColor(args.vg, nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF));
        nvgStroke(args.vg);

        nvgRestore(args.vg);

        
        module->readyToDisplay = false;
    }
};



struct TestWidget : ModuleWidget {
	TestWidget(Test *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/test.svg")));

        {
			TestDisplay *display = new TestDisplay();
			display->module = module;
			display->box.pos = Vec(0, 44);
			display->box.size = Vec(box.size.x, 140);
			addChild(display);
		}

        addParam(createParam<SLMSmallKnobGreen>(Vec(15, 270), module, Test::WINDOW_CENTER_PARAM));
        addParam(createParam<SLMSmallKnobGreen>(Vec(15, 300), module, Test::WINDOW_CENTER_ATTENUERTER));
        addInput(createInput<SLMInputPort>(Vec(15, 350), module, Test::WINDOW_CENTER_CV));
        addParam(createParam<SLMSmallKnobGreen>(Vec(60, 270), module, Test::WINDOW_WIDTH_PARAM));
        addParam(createParam<SLMSmallKnobGreen>(Vec(60, 300), module, Test::WINDOW_WIDTH_ATTENUERTER));
        addInput(createInput<SLMInputPort>(Vec(60, 350), module, Test::WINDOW_WIDTH_CV));
        addInput(createInput<SLMInputPort>(Vec(17, 185), module, Test::CH1_INPUT));
        addInput(createInput<SLMInputPort>(Vec(57, 185), module, Test::CH2_INPUT));
        addInput(createInput<SLMInputPort>(Vec(17, 225), module, Test::SYNC_INPUT));

        addOutput(createOutput<SLMOutputPort>(Vec(129, 327), module, Test::MAIN_OUTPUT));
	}
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelTest = createModel<Test, TestWidget>("test");
