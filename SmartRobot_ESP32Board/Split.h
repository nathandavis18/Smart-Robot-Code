#ifndef mSplit
#define mSplit
#pragma warning(disable : 4996)

#include "MyString.h"
#include "MyArray.h"

namespace sr{
  template<unsigned int size>
  void split(const MyString& data, MyArray<MyString, size>& arr, char delimiter = ','){
      unsigned int currentPos = 0;
      if (data.charAt(currentPos) == '[' || data.charAt(currentPos) == '{')
          ++currentPos;
      
      unsigned int dataBegin = currentPos;
      while (currentPos < data.length()) {
		  currentPos = data.indexOf(delimiter, currentPos);

          if (currentPos != dataBegin && currentPos < data.length()) {
              arr.insert(data.substring(dataBegin, currentPos - dataBegin));
              ++currentPos;
              dataBegin = currentPos; // Move past the delimiter
          }
      }

	  currentPos = data.length();
      if(data.charAt(data.length() - 1) == ']' || data.charAt(data.length() - 1) == '}') {
		  currentPos = data.length() - 1;
	  }

      if (currentPos != dataBegin) {
		  arr.insert(data.substring(dataBegin, currentPos - dataBegin));
      }
  }
}

#endif //mSplit