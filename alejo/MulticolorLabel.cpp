/*
 * Multicolor Label
 *
 */

#include "MulticolorLabel.h"

namespace alejo
{

	MulticolorLabel * MulticolorLabel::create(const char *s, const char *fontFile, const ColorMap &colorMap)
	{
		MulticolorLabel *label = new MulticolorLabel();
		if (label && label->initWithColorMap(s, fontFile, colorMap))
		{
			label->autorelease();
			return label;
		}
	
		CC_SAFE_DELETE(label);
		return nullptr;
	}

	MulticolorLabel * MulticolorLabel::create(const char *s, const char *fontFile)
	{
		ColorMap colorMap;
		colorMap["$"] = cocos2d::ccc3(255, 255, 255);
	
		return MulticolorLabel::create(s, fontFile, colorMap);
	}

	bool MulticolorLabel::initWithColorMap(const char *s, const char *fontFile, const ColorMap &colorMap)
	{
		if (!cocos2d::CCLabelBMFont::initWithString("", fontFile))
			return false;
	
		this->colorMap = colorMap;
		this->setColoredString(s);
	
		return true;
	}

	MulticolorLabel::MulticolorLabel(void)
		: fmtString(nullptr)
	{
	}

	MulticolorLabel::~MulticolorLabel(void)
	{
		CC_SAFE_DELETE_ARRAY(fmtString);
	}

	void MulticolorLabel::setString(const char *s)
	{
		cocos2d::CCLabelBMFont::setString(s);
		this->replaceFormatString(s);
		this->coloredParts.clear();

		cocos2d::ccColor3B color = this->getColor();
		this->setColor(color);

		cocos2d::CCObject *obj;
		CCARRAY_FOREACH(this->m_pChildren, obj)
		{
			static_cast<cocos2d::CCSprite *>(obj)->setColor(color);
		}
	}

	void MulticolorLabel::setString(const char *s, bool updateLabel)
	{
		cocos2d::CCLabelBMFont::setString(s, updateLabel);
		this->replaceFormatString(s);
		this->coloredParts.clear();

		cocos2d::ccColor3B color = this->getColor();
		this->setColor(color);

		cocos2d::CCObject *obj;
		CCARRAY_FOREACH(this->m_pChildren, obj)
		{
			static_cast<cocos2d::CCSprite *>(obj)->setColor(color);
		}
	}

	void MulticolorLabel::setColoredString(const char *s)
	{
		static char buff[1024];
		size_t idx = 0;
	
		std::vector<ColoredPart> parts;

		ColoredPart part;
		bool inColoredPart = false;
		size_t length = strlen(s);
		for (size_t i = 0; i < length; ++i)
		{
			char c = s[i];
			if (c == '>')
			{
				if (inColoredPart)
				{
					if (s[i + 1] == '>') //escape
					{
						buff[idx++] = c;
						++i;
					}
					else
					{
						inColoredPart = false;
						part.endIndex = idx;
						parts.push_back(part);
					}
				}
				else
				{
					buff[idx++] = c;
				}
			}
			else if (c == '$') //format character
			{
				c = s[i + 1];
				if (c == '\0') // string ends
				{
					buff[idx++] = c;
				}
				else if (c == '$') // escape $
				{
					++i;
					buff[idx++] = c;
				}
				else
				{
					++i;
					size_t startIndex;
					const char *tag = getColorTag(s, length, i, startIndex);
					if (tag == nullptr)
					{
						buff[idx++] = c;
					}
					else
					{
						if (tag[0] == '#') // color code
						{
							int code = 0;
							sscanf(tag+1, "%6x", &code);
							part.startIndex = idx;
							part.color = getColorFromInt(code);
							inColoredPart = true;
						}
						else
						{
							auto it = colorMap.find(tag);
							if (it == colorMap.end())
							{
								//buff[idx++] = c;
								inColoredPart = true;
							}
							else
							{
								part.startIndex = idx;
								part.color = it->second;
								inColoredPart = true;
							}
						} // found named color
					} // found valid tag
				} // found tag
			} // found $
			else
			{
				buff[idx++] = c;
			}
		}

		if (inColoredPart)
		{
			part.endIndex = idx + 1;
			parts.push_back(part);
		}

		buff[idx] = '\0';
		cocos2d::CCLabelBMFont::setString(buff);
		this->replaceFormatString(s);
		this->coloredParts = parts;

		auto itColor = this->colorMap.find("$");
		if (itColor != this->colorMap.end())
			this->setColor(itColor->second);

		for (auto it = parts.begin(), itEnd = parts.end(); it != itEnd; ++it)
		{
			ColoredPart &part = *it;
		
			for (size_t i = part.startIndex; i < part.endIndex; ++i)
				static_cast<cocos2d::CCSprite *>(this->getChildByTag(i))->setColor(part.color);
		}
	}

	void MulticolorLabel::setColor(const cocos2d::ccColor3B &color)
	{
		cocos2d::CCLabelBMFont::setColor(color);
		this->colorMap["$"] = color;

		for (auto it = this->coloredParts.begin(), itEnd = this->coloredParts.end(); it != itEnd; ++it)
		{
			ColoredPart &part = *it;
		
			for (size_t i = part.startIndex; i < part.endIndex; ++i)
				static_cast<cocos2d::CCSprite *>(this->getChildByTag(i))->setColor(part.color);
		}
	}

	void MulticolorLabel::unformat(void)
	{
		this->setColoredString(this->getString());
	}

	const char * MulticolorLabel::getColorTag(const char *s, size_t length, size_t &i, size_t &startIndex)
	{
		static char buff[256];
		size_t idx = 0;
	
		for (size_t j = i; j < length; ++j)
		{
			char c = s[j];
			if (c == '\0') // string ends
				return nullptr;
		
			if (c == '<')
			{
				c = s[i + 1];
				if (c == '\0') // string ends
					return nullptr;
		
				buff[idx] = '\0';
				i = j;
				startIndex = i + 1;
				return buff;
			}
			else
			{
				buff[idx++] = c;
			}
		}

		return buff;
	}

	cocos2d::ccColor3B MulticolorLabel::getColorFromInt(int value)
	{
		cocos2d::ccColor3B color;
		color.r = (value & 0xFF0000) >> 16;
		color.g = (value & 0x00FF00) >> 8;
		color.b = (value & 0x0000FF);
	
		return color;
	}

	void MulticolorLabel::replaceFormatString(const char *newString)
	{
		CC_SAFE_DELETE_ARRAY(fmtString);

	#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		fmtString = _strdup(newString);
	#else
		fmtString = strdup(newString);
	#endif
	}

} //namespace alejo
