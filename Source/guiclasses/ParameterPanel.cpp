#include "ParameterPanel.h"
#include "../LuaLink.h"
#include "../PluginProcessor.h"

ParamSlider::ParamSlider (LuaProtoplugJuceAudioProcessor* pfxIn, int indexIn)
	: index (indexIn), pfx (pfxIn)
{
	//Slider::setTextBoxIsEditable(false);
}

String ParamSlider::getTextFromValue (double /*value must be set in pfx*/)
{
	return pfx->getParameterText(index);
}

double ParamSlider::getValueFromText (const String& text)
{
	double d;
	if (pfx->parameterText2Double(index, text, d))
		return d;
	return Slider::getValueFromText(text);
}

void ParamPanelContent::paint (Graphics& g)
{
	g.fillAll (Colour(0xffffffff));
}

ParameterPanel::ParameterPanel (LuaProtoplugJuceAudioProcessor* processorIn)
	: processor (processorIn)
{
	content.reset(new ParamPanelContent());
	content->setBounds(0, 0, 220, NPARAMS*36+36);
	for (int i=0; i<NPARAMS; i++) {
		labels[i].reset(new Label());
		labels[i]->setEditable (false, false, false);
		labels[i]->setBounds(10, i*36, 100, 22);
		content->addAndMakeVisible(labels[i].get());
		sliders[i].reset(new ParamSlider(processor, i));
		sliders[i]->setSliderStyle (Slider::LinearBar);
		sliders[i]->setBounds(110, i*36, getWidth()-130, 22);
		sliders[i]->setRange(0, 1.0);
		sliders[i]->setValue(processor->params[i], dontSendNotification);
		sliders[i]->updateText();
		sliders[i]->addListener(this);
		sliders[i]->setColour(Slider::ColourIds::textBoxTextColourId, Colours::black);
		content->addAndMakeVisible(sliders[i].get());
	}
	updateNames();
	setViewedComponent (content.get(), false);
}

void ParameterPanel::resized()
{
	content->setSize(std::max(getWidth()-getLookAndFeel().getDefaultScrollbarWidth(),320), NPARAMS*36+36);
	for (int i=0; i<NPARAMS; i++) {
		sliders[i]->setSize(std::max(getWidth()-130,200), 22);
	}
	// work around shit
	setViewPosition(getViewPosition().x, getViewPosition().y+1);
	setViewPosition(getViewPosition().x, getViewPosition().y-1);
}

void ParameterPanel::sliderDragStarted (Slider* sliderThatWasMoved)
{
	for (int i=0; i<NPARAMS; i++)
		if (sliders[i].get()==sliderThatWasMoved) {
			processor->beginParameterChangeGesture(i);
			break;
		}
}

void ParameterPanel::sliderDragEnded (Slider* sliderThatWasMoved)
{
	for (int i=0; i<NPARAMS; i++)
		if (sliders[i].get()==sliderThatWasMoved) {
			processor->endParameterChangeGesture(i);
			break;
		}
}

void ParameterPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
	for (int i=0; i<NPARAMS; i++)
		if (sliders[i].get()==sliderThatWasMoved) {
			processor->setParameterNotifyingHost(i, (float)sliderThatWasMoved->getValue());
			sliders[i]->updateText();
			break;
		}
}

void ParameterPanel::updateNames()
{
	for (int i=0; i<NPARAMS; i++) {
		String s = processor->luli->getParameterName(i);
		if (s.isEmpty()) {
			s = "nameless";
			labels[i]->setColour(Label::textColourId, Colours::grey);
		} else
			labels[i]->setColour(Label::textColourId, Colours::black);
		labels[i]->setText(String::formatted("%d. ",i) + s, dontSendNotification);
	}
}

void ParameterPanel::paramsChanged()
{
	for (int i=0; i<NPARAMS; i++) {
		sliders[i]->setValue(processor->params[i], dontSendNotification);
		sliders[i]->updateText();
	}
}

void ParameterPanel::paint (Graphics& g)
{
	g.fillAll (Colour(0xffffffff));
}
