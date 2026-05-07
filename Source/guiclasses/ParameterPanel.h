#pragma once

#include <JuceHeader.h>
#include "../ProtoplugConstants.h"

class LuaProtoplugJuceAudioProcessor;

class ParamSlider : public Slider
{
public:
	ParamSlider (LuaProtoplugJuceAudioProcessor* pfx, int index);

	String getTextFromValue (double value) override;
	double getValueFromText (const String& text) override;

private:
	int index;
	LuaProtoplugJuceAudioProcessor* pfx;
};

class ParamPanelContent : public Component
{
public:
	void paint (Graphics& g) override;
};

class ParameterPanel : public Viewport, public Slider::Listener
{
public:
	ParameterPanel (LuaProtoplugJuceAudioProcessor* processor);

	void resized() override;
	void sliderDragStarted (Slider* sliderThatWasMoved) override;
	void sliderDragEnded (Slider* sliderThatWasMoved) override;
	void sliderValueChanged (Slider* sliderThatWasMoved) override;
	void updateNames();
	void paramsChanged();
	void paint (Graphics& g) override;

private:
	std::unique_ptr<Component> content;
	std::unique_ptr<Slider> sliders[NPARAMS];
	std::unique_ptr<Label> labels[NPARAMS];
	LuaProtoplugJuceAudioProcessor* processor;
};
