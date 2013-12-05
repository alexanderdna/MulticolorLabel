/*
 * Multicolor Label
 *
 */

#ifndef __ALEJO_MULTICOLORLABEL_H__
#define __ALEJO_MULTICOLORLABEL_H__

#include "cocos2d.h"

namespace alejo
{

	class MulticolorLabel : public cocos2d::CCLabelBMFont
	{
	public:
		typedef std::map<std::string, cocos2d::ccColor3B> ColorMap;

	protected:
		struct ColoredPart
		{
			size_t startIndex;
			size_t endIndex;
			cocos2d::ccColor3B color;
		};
	
	public:
		static MulticolorLabel * create(const char *s, const char *fontFile, const ColorMap &colorMap);
		static MulticolorLabel * create(const char *s, const char *fontFile);

	protected:
		bool initWithColorMap(const char *s, const char *fontFile, const ColorMap &colorMap);
	
	protected:
		ColorMap colorMap;
		const char *fmtString;
		std::vector<ColoredPart> coloredParts;

	public:
		MulticolorLabel(void);
		virtual ~MulticolorLabel(void);
	
	public:
		void setString(const char *s);
		void setString(const char *s, bool updateLabel);
		void setColoredString(const char *s);

		void setColor(const cocos2d::ccColor3B &color);

		void unformat(void);
	
	protected:
		const char * getColorTag(const char *s, size_t length, size_t &i, size_t &startIndex);
		cocos2d::ccColor3B getColorFromInt(int value);

		void replaceFormatString(const char *newString);
	};

}

#endif//__ALEJO_MULTICOLORLABEL_H__
