#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "guiclasses/ProtoWindow.h"

class ProtoPopout;

class LuaProtoplugJuceAudioProcessorEditor  :	public AudioProcessorEditor,
												public Button::Listener
{
public:
	LuaProtoplugJuceAudioProcessorEditor (LuaProtoplugJuceAudioProcessor* ownerFilter);
	~LuaProtoplugJuceAudioProcessorEditor();
	
	void paint (Graphics& g);
	void resized();
	void handleCommandMessage(int com);
	void buttonClicked(Button *);

	void popOut();
	void popIn();
    LuaProtoplugJuceAudioProcessor *processor;

private:
	void handleProtoplugDirectoryChosen(const File& chosen);

	ProtoWindow content; // the actual gui is in there
	std::unique_ptr<ProtoPopout> poppedWin;
	std::unique_ptr<FileChooser> directoryChooser;
	TextButton yank;
	TextButton popin;
	TextButton locateFiles;
};
