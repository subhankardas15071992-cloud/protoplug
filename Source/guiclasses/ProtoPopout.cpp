#include "ProtoPopout.h"
#include "../PluginEditor.h"

void ProtoPopout::closeButtonPressed()
{
	vstPanel->postCommandMessage(MSG_POPOUT);
	// todo, check if everything is crashed and force close
}
