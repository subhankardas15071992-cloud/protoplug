#include "HintedFeel.h"

static Typeface::Ptr getBuiltInTypeface(const String& faceName)
{
	for (unsigned int i = 0; i < sizeof (protoFonts) / sizeof (protoFonts[0]); ++i)
		if (faceName == protoFonts[i].name)
			return Typeface::createSystemTypefaceFor(protoFonts[i].data, (size_t) protoFonts[i].size);

	return nullptr;
}

FontDataMap HintedFeel::faces = FontDataMap();

Typeface::Ptr HintedFeel::getTypefaceForFont (const Font& font)
{
	String faceName (font.getTypefaceName());

	if (faceName.endsWith("_hinted_"))
	{
		faceName = faceName.dropLastCharacters(8);

		auto iter = faces.find(faceName);
		if (iter == faces.end())
			iter = faces.insert({ faceName, getBuiltInTypeface(faceName) }).first;

		if (iter->second)
			return iter->second;
	}

	Font f (font);
	f.setTypefaceName (faceName);
	return LookAndFeel::getTypefaceForFont (f);
}

HintedFeel::~HintedFeel()
{
	// possibly use a static ReferenceCountedObjectPtr<FontDataMap> ?
	// the current implementation may be more efficient
}
