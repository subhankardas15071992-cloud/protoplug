/*
  ==============================================================================

    Dockable.cpp
    Created: 13 Apr 2014 4:37:52pm
    Author:  pac

  ==============================================================================
*/

#include "Dockable.h"
#include "../PluginProcessor.h"

DockablePopout::DockablePopout (Dockable* dadIn,
								const String& name,
								Colour backgroundColour,
								int requiredButtons,
								bool addToDesktop)
	: DocumentWindow(name, backgroundColour, requiredButtons, addToDesktop),
	  dad (dadIn)
{
}

void DockablePopout::closeButtonPressed()
{
	dad->postCommandMessage(1);
}

Dockable::Dockable (Component* contentIn, String nameIn, LuaProtoplugJuceAudioProcessor* processorIn)
	: content (contentIn), name (nameIn), processor (processorIn)
{
	addAndMakeVisible(content);
}

void Dockable::paint (Graphics& g)
{
	g.fillAll (Colours::white);
	if (docwin==0) return;
	g.fillAll();
	g.setColour(Colours::grey);
	g.drawText(name + " window popped out !", g.getClipBounds(), Justification::centred, false);
}

void Dockable::resized()
{
	if (docwin==0)
		content->setBounds(0, 0, getWidth(), getHeight());
}

void Dockable::handleCommandMessage (int com)
{
	if (com==1 && docwin==0)
		popOut();
	else if (com==1 && docwin!=0)
		popIn();
}

void Dockable::popOut()
{
	docwin.reset(new DockablePopout(this, name, Colours::white, DocumentWindow::allButtons, true));
	docwin->setAlwaysOnTop(processor->alwaysontop);
	docwin->setResizable(true, false);
	docwin->setUsingNativeTitleBar(true);
	docwin->setContentNonOwned(content, true);
	//processor->popout = true;
	//docwin->setContentComponentSize(processor->lastUIWidth, processor->lastUIHeight);
	docwin->setTopLeftPosition(processor->lastPopoutX, processor->lastPopoutY);
	//content.setPoppedOut(true);
	docwin->setVisible(true);
	//setSize (280, 130);
	//yank.setVisible(true);
	//popin.setVisible(true);
	//content.takeFocus();
	resized();
}

void Dockable::popIn()
{
	//processor->popout = false;
	//int w=processor->lastUIWidth, h=processor->lastUIHeight;
	addAndMakeVisible(content);
	//content.setPoppedOut(false);
	//setSize (w,h);
	content->setSize (getWidth(), getHeight());
	docwin.reset();
	//yank.setVisible(false);
	//content.takeFocus();
	//popin.setVisible(false);
	resized();
}

void Dockable::setAlwaysOnTop (bool aot)
{
	if (docwin==0) return;
	docwin->setAlwaysOnTop(aot);
}

bool Dockable::isPoppedOut()
{
	return (docwin!=0);
}

void Dockable::bringWindowToFront()
{
	if (docwin==0) return;
	docwin->toFront(true);
}
