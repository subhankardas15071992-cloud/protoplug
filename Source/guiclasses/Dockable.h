/*
  ==============================================================================

    Dockable.h
    Created: 13 Apr 2014 3:42:04pm
    Author:  pac

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LuaProtoplugJuceAudioProcessor;
class Dockable;

class DockablePopout  :	public DocumentWindow
{
public:
	DockablePopout (Dockable* dad,
					const String& name,
					Colour backgroundColour,
					int requiredButtons,
					bool addToDesktop = true);

	void closeButtonPressed() override;

private:
	Dockable* dad;
};

class Dockable : public Component
{
public:
	Dockable (Component* content, String name, LuaProtoplugJuceAudioProcessor* processor);

	void paint (Graphics& g) override;
	void resized() override;
	void handleCommandMessage (int com) override;

	void popOut();
	void popIn();
	void setAlwaysOnTop (bool aot);
	bool isPoppedOut();
	void bringWindowToFront();

private:
	Component* content;
	std::unique_ptr<DockablePopout> docwin;
	String name;
	LuaProtoplugJuceAudioProcessor* processor;
};
