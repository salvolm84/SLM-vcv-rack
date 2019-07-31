#include "common.hpp"

#define BUFFER_SIZE 100 * 1024

struct TheTailor : Module {
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
        OVERFLOW_ALERT_LIGHT,
		NUM_LIGHTS
	};

    int sampleCounter = 0;
    int samplesPerPeriod = 0;
    int syncCount = 0;

    bool previousAutoSyncLevel = false;

    dsp::SchmittTrigger schmittTrigger;
    bool readyToDisplay = false;
    
    float displayBuffer[BUFFER_SIZE] = {0.0f};

	TheTailor() {
		// Configure the module
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(WINDOW_CENTER_PARAM, 0.0f, 1.0f, 0.5f, "Window Center");
        configParam(WINDOW_WIDTH_PARAM, 0.0f, 1.0f, 0.3f, "Window Width");
        configParam(WINDOW_CENTER_ATTENUERTER, -1.0f, 1.0f, 0.0f, "Window Center CV Attenuerter");
        configParam(WINDOW_WIDTH_ATTENUERTER, -1.0f, 1.0f, 0.0f, "Window Width CV Attenuerter");
	}

	void process(const ProcessArgs &args) override {

        bool sync = false;

        if (!inputs[SYNC_INPUT].isConnected()) {
            // we are using auto sync
            bool currentAutoSyncLevel = inputs[CH1_INPUT].getVoltage() > 0.0f;
            sync = currentAutoSyncLevel && (!previousAutoSyncLevel);
            previousAutoSyncLevel = currentAutoSyncLevel;
        } else {
            sync = schmittTrigger.process(rescale(inputs[SYNC_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f));
        }

        if (sync) {
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

        if (sampleCounter < BUFFER_SIZE) {
            displayBuffer[sampleCounter++] = outputs[MAIN_OUTPUT].getVoltage();
        }

        lights[OVERFLOW_ALERT_LIGHT].setBrightness(sampleCounter < BUFFER_SIZE ? 0.0f : 1.0f);

	}
};

struct TheTailorDisplay : TransparentWidget {
	TheTailor *module;

    void draw(const DrawArgs &args) override {
        if (!module)
			return;

        if (!module->readyToDisplay)
            return;

        int size = module->samplesPerPeriod;
        
        float width = box.size.x;
        float deltaTime = width / (float)size;

        // float drawHeight = 100.0f;
        float center = box.size.y / 2.0f;
        float gain = - 4.0f;
        
        nvgSave(args.vg);
        nvgBeginPath(args.vg);

        nvgMoveTo(args.vg, 0, center + gain * module->displayBuffer[0]);
        
        for (int i = 0; i < size; i++) {
            nvgLineTo(args.vg, (float)i * deltaTime, center + gain * module->displayBuffer[i]);
        }

        nvgStrokeWidth(args.vg, 1.0f);
        nvgStrokeColor(args.vg, nvgRGBA(0xEB, 0xEB, 0xEB, 0xFF));
        nvgStroke(args.vg);

        nvgRestore(args.vg);

        module->readyToDisplay = false;
    }
};



struct TheTailorWidget : ModuleWidget {
	TheTailorWidget(TheTailor *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/the-tailor.svg")));

        addChild(createWidget<SLMScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<SLMScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<SLMScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<SLMScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        {
			TheTailorDisplay *display = new TheTailorDisplay();
			display->module = module;
			display->box.pos = Vec(10, 38);
			display->box.size = Vec(130, 76);
			addChild(display);
		}

        addParam(createParam<SLMBigKnobGreen>(Vec(19, 138), module, TheTailor::WINDOW_CENTER_PARAM));
        addParam(createParam<SLMTrimmer>(Vec(10, 190), module, TheTailor::WINDOW_CENTER_ATTENUERTER));
        addInput(createInput<SLMInputPort>(Vec(19, 272), module, TheTailor::WINDOW_CENTER_CV));

        addParam(createParam<SLMBigKnobRed>(Vec(93, 138), module, TheTailor::WINDOW_WIDTH_PARAM));
        addParam(createParam<SLMTrimmer>(Vec(116, 190), module, TheTailor::WINDOW_WIDTH_ATTENUERTER));
        addInput(createInput<SLMInputPort>(Vec(105, 272), module, TheTailor::WINDOW_WIDTH_CV));

        addInput(createInput<SLMInputPort>(Vec(39, 228), module, TheTailor::CH1_INPUT));
        addInput(createInput<SLMInputPort>(Vec(85, 228), module, TheTailor::CH2_INPUT));

        addInput(createInput<SLMInputPort>(Vec(62, 272), module, TheTailor::SYNC_INPUT));

        addOutput(createOutput<SLMOutputPort>(Vec(62, 310), module, TheTailor::MAIN_OUTPUT));
        addChild(createLight<MediumLight<SLMRedLight>>(Vec(71, 133), module, TheTailor::OVERFLOW_ALERT_LIGHT));
	}
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelTheTailor = createModel<TheTailor, TheTailorWidget>("the-tailor");
