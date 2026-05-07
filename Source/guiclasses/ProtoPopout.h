#pragma once

#include <JuceHeader.h>

class LuaProtoplugJuceAudioProcessorEditor;

class ProtoPopout  :	public DocumentWindow
{
public:
	ProtoPopout (	LuaProtoplugJuceAudioProcessorEditor* vstPanel,
					const String& name,
					Colour backgroundColour,
					int requiredButtons,
					bool addToDesktop = true)
		: DocumentWindow(name, backgroundColour, requiredButtons, addToDesktop),
		  vstPanel(vstPanel)
	{
	}

	void closeButtonPressed() override;

private:
	LuaProtoplugJuceAudioProcessorEditor* vstPanel;
};
