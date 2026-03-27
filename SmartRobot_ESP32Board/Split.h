#pragma once
#pragma warning(disable : 4996)

#include "EmbeddedString.h"
#include "Array.h"

namespace sr
{
	template<unsigned int size>
	void split(const HeaderString& data, MyArray<SplitsString, size>& arr, const char delimiter = ',')
	{
		unsigned int currentPos = 0;
		if (data.char_at(currentPos) == '[' || data.char_at(currentPos) == '{')
			++currentPos;

		unsigned int dataBegin = currentPos;
		while (currentPos < data.length())
		{
			currentPos = data.indexOf(delimiter, currentPos);

			if (currentPos != dataBegin && currentPos < data.length())
			{
				arr.insert_back(data.substring<SplitsString>(dataBegin, currentPos - dataBegin));
				++currentPos;
				dataBegin = currentPos; // Move past the delimiter
			}
		}

		currentPos = data.length();
		if (data.char_at(data.length() - 1) == ']' || data.char_at(data.length() - 1) == '}')
		{
			currentPos = data.length() - 1;
		}

		if (currentPos != dataBegin)
		{
			arr.insert_back(data.substring<SplitsString>(dataBegin, currentPos - dataBegin));
		}
	}
}