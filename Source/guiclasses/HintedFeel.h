#pragma once

#include <JuceHeader.h>
#include "../vflib/BinaryDejaVu.h"
#include "../vflib/BinarySourceCodePro.h"
#include <map>

struct BuiltInFont
{
    const char* name;
	int size;
    const char* data;
};
const BuiltInFont protoFonts[] =
{
	{ "DejaVu Sans Mono",
		BinaryDejaVu::dejavusansmono_ttfSize,
		BinaryDejaVu::dejavusansmono_ttf },
	{ "Source Code Pro",
		BinarySourceCodePro::sourcecodeproregular_ttfSize,
		BinarySourceCodePro::sourcecodeproregular_ttf }
};

typedef std::map<String, Typeface::Ptr> FontDataMap;

class HintedFeel : public LookAndFeel_V3
{
public:
	HintedFeel()	{ }
	~HintedFeel();
	Typeface::Ptr getTypefaceForFont (const Font& font) override;
	static FontDataMap faces;

	Font getPopupMenuFont()
	{
		return Font (15.0f);
	}
	bool 	areScrollbarButtonsVisible () {return true;}
	void drawTooltip (Graphics& g, const String& text, int width, int height)
	{
		// this is just a non-bold version of the parent
		g.fillAll (findColour (TooltipWindow::backgroundColourId));
		#if ! JUCE_MAC
			g.setColour (findColour (TooltipWindow::outlineColourId));
			g.drawRect (0, 0, width, height, 1);
		#endif
        AttributedString s;
        s.setJustification (Justification::centred);
        s.append (text, Font (13.0f), findColour (TooltipWindow::textColourId));
        TextLayout tl;
        tl.createLayoutWithBalancedLineLengths (s, (float) 400);
		tl.draw (g, juce::Rectangle<float> ((float) width, (float) height));
	}

	void getIdealPopupMenuItemSize (const String& text, bool isSeparator,
									int standardMenuItemHeight, int& idealWidth, int& idealHeight)
	{
		LookAndFeel_V2::getIdealPopupMenuItemSize(text, isSeparator, standardMenuItemHeight, idealWidth, idealHeight);
		idealHeight += 4;
	}
};
