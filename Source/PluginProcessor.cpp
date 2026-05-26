/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "LuaLink.h"
#include "PluginEditor.h"
#include "ProtoplugDir.h"


class ProtoplugParameter : public AudioProcessorParameter
{
public:
	ProtoplugParameter (LuaProtoplugJuceAudioProcessor& ownerIn, int indexIn)
		: AudioProcessorParameter (1), owner (ownerIn), index (indexIn)
	{
	}

	float getValue() const override
	{
		return owner.getParameter(index);
	}

	void setValue (float newValue) override
	{
		owner.setParameter(index, newValue);
	}

	float getDefaultValue() const override
	{
		return owner.getParameterDefaultValue(index);
	}

	String getName (int maximumStringLength) const override
	{
		auto name = owner.getParameterName(index);
		return maximumStringLength > 0 ? name.substring(0, maximumStringLength) : name;
	}

	String getLabel() const override
	{
		return {};
	}

	String getText (float /*normalisedValue*/, int maximumStringLength) const override
	{
		auto text = owner.getParameterText(index);
		return maximumStringLength > 0 ? text.substring(0, maximumStringLength) : text;
	}

	float getValueForText (const String& text) const override
	{
		double parsedValue = 0.0;
		if (owner.parameterText2Double(index, text, parsedValue))
			return (float) parsedValue;
		return text.getFloatValue();
	}

private:
	LuaProtoplugJuceAudioProcessor& owner;
	int index;
};


//==============================================================================
LuaProtoplugJuceAudioProcessor::LuaProtoplugJuceAudioProcessor()
#ifdef _PROTOGEN
	: AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true))
#else
	: AudioProcessor (BusesProperties().withInput ("Input", AudioChannelSet::stereo(), true)
									 .withOutput ("Output", AudioChannelSet::stereo(), true))
#endif
{
    lastUIWidth = 670;
    lastUIHeight = 455;
	lastUISplit = 455-46;
	lastUIPanel = 0;
	lastPopoutX = lastPopoutY = 60;
	lastUIFontSize = -1;
	popout = alwaysontop = liveMode = false;
	for (int i=0; i<NPARAMS; i++)
		params[i] = 0.5;
	for (int i=0; i<NPARAMS; i++)
		addParameter(new ProtoplugParameter(*this, i));
	chunk = 0;
	luli = new LuaLink(this);
}

LuaProtoplugJuceAudioProcessor::~LuaProtoplugJuceAudioProcessor()
{
	delete luli;
	if (chunk)
		delete[] chunk;
}

//==============================================================================

float LuaProtoplugJuceAudioProcessor::getParameter (int index)
{
	if (index >= NPARAMS)
		return 0.f;
	return (float)params[index];
}

void LuaProtoplugJuceAudioProcessor::setParameter (int index, float newValue)
{
	if (index >= NPARAMS)
		return;
	params[index] = (double) newValue;
	luli->paramChanged(index);
	if (getActiveEditor())
		lastOpenedEditor->msg_ParamsChanged = true;
}

const String LuaProtoplugJuceAudioProcessor::getParameterName (int index)
{
	if (index >= NPARAMS)
		return {};
	return luli->getParameterName(index);
}

const String LuaProtoplugJuceAudioProcessor::getParameterText (int index)
{
	if (index >= NPARAMS)
		return {};
	String s = luli->getParameterText(index);
	if (s.isEmpty())
		s = String(params[index], 4);
	return s;
}

bool LuaProtoplugJuceAudioProcessor::parameterText2Double (int index, String text, double &d)
{
	if (index >= NPARAMS)
		return false;
	return luli->parameterText2Double(index, text, d);
}

void LuaProtoplugJuceAudioProcessor::beginParameterChangeGesture (int index)
{
	if (index >= NPARAMS)
		return;
	if (auto* parameter = getParameters()[index])
		parameter->beginChangeGesture();
}

void LuaProtoplugJuceAudioProcessor::endParameterChangeGesture (int index)
{
	if (index >= NPARAMS)
		return;
	if (auto* parameter = getParameters()[index])
		parameter->endChangeGesture();
}

void LuaProtoplugJuceAudioProcessor::setParameterNotifyingHost (int index, float newValue)
{
	if (index >= NPARAMS)
		return;
	if (auto* parameter = getParameters()[index])
		parameter->setValueNotifyingHost(newValue);
	else
		setParameter(index, newValue);
}

double LuaProtoplugJuceAudioProcessor::getTailLengthSeconds() const
{
    return luli->getTailLengthSeconds();
}

bool LuaProtoplugJuceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
	auto isSupportedChannelCount = [] (int channels)
	{
		return channels >= 1 && channels <= MAX_AUDIO_CHANNELS;
	};

	const int numInputChannels = layouts.inputBuses.size() > 0 ? layouts.inputBuses[0].size() : 0;
	const int numOutputChannels = layouts.outputBuses.size() > 0 ? layouts.outputBuses[0].size() : 0;

#ifdef _PROTOGEN
	if (layouts.inputBuses.size() != 0 || layouts.outputBuses.size() != 1)
		return false;

	return numInputChannels == 0 && isSupportedChannelCount (numOutputChannels);
#else
	if (layouts.inputBuses.size() != 1 || layouts.outputBuses.size() != 1)
		return false;

	return numInputChannels == numOutputChannels && isSupportedChannelCount (numOutputChannels);
#endif
}

//==============================================================================

void LuaProtoplugJuceAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
#ifdef _PROTOGEN
	buffer.clear();
#endif

	luli->processBlock(buffer, midiMessages, getPlayHead());
	midiMessages.data;
}

AudioProcessorEditor* LuaProtoplugJuceAudioProcessor::createEditor()
{
    return new LuaProtoplugJuceAudioProcessorEditor (this);
}

//==============================================================================
void LuaProtoplugJuceAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	// if code editor is open, flush its code to luli buffer
	if (getActiveEditor())
		lastOpenedEditor->saveCode();

	// if lua save() is overridden, call it and store the script's custom data
	luli->saveData = luli->save();

	int sz_script = luli->code.length()*2;
	int sz_user = luli->saveData.length()*2;
	int sz_total = 3*4 + 8*NPARAMS + sz_script + sz_user + 8;

	if (chunk)
		delete[] chunk;
	chunk = new char[sz_total];

	int *pi = (int*)chunk;
	*pi++ = NPARAMS;					// store number of parameters
	double *pd = (double*) pi;
	for (int i=0; i<NPARAMS; i++)
		*pd++ = params[i];				// store param values
	pi = (int*)pd;
	*pi++ = sz_script;					// store size of code
	char *pc = (char*)pi;
	strcpy(pc, luli->code.getCharPointer());				// store code
	pc += sz_script;
	pi = (int*)pc;
	*pi++ = sz_user;					// store size of lua saveable string
	pc = (char*)pi;
	strcpy(pc, luli->saveData.getCharPointer());			// store lua saveable string

	destData.append(chunk, sz_total);
	return;
}

void LuaProtoplugJuceAudioProcessor::setStateInformation (const void* data, int /*sizeInBytes*/)
{
	int *pi = (int*)data;
	int numparams = *pi++;				// get number of parameters
	double *pd = (double*) pi;
	for (int i=0; i<numparams; i++)
		if (i<NPARAMS)
			params[i] = *pd++;			// get param values
		else
			*pd++;
	pi = (int*)pd;
	int sz_script = *pi++;				// get size of code
	char *pc = (char*)(pi);
	luli->code = pc;					// get code
	luli->saveData = {};
	if (ProtoplugDir::Instance()->found)
		luli->compile();
	else
		luli->addToLog("could not compile script because the ProtoplugFiles directory is missing or incomplete");
	
	pc += sz_script;
	pi = (int*)pc;
	int sz_user = *pi++;				// get size of lua saveable string
	pc = (char*)pi;
	if (sz_user>0) {
		luli->saveData = pc;			// get lua saveable string
		luli->load(luli->saveData);
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LuaProtoplugJuceAudioProcessor();
}


ProtoWindow *LuaProtoplugJuceAudioProcessor::getProtoEditor()
{
	if (getActiveEditor())
		return lastOpenedEditor;
	else
		return 0;
}

void LuaProtoplugJuceAudioProcessor::setProtoEditor(ProtoWindow *_ed)
{
	lastOpenedEditor = _ed;
}
