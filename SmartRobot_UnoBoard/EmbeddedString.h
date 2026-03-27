#ifndef _EmbeddedString_
#define _EmbeddedString_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#pragma warning(disable : 4996)

namespace sr
{
	namespace details
	{
		template<unsigned int capacity>
		class EmbeddedString
		{
		public:
			// Constructors
			EmbeddedString() : mData(""), mLength(0) {}
			EmbeddedString(const char* data) : mLength(strlen(data))
			{
				strcpy(mData, data);
			}

			// Assignment Operators
			EmbeddedString& operator=(const char* data)
			{
				strcpy(mData, data);
				mLength = strlen(data);
				return *this;
			}

			EmbeddedString& operator=(const EmbeddedString& other)
			{
				strcpy(mData, other.mData);
				mLength = other.mLength;
				return *this;
			}

			EmbeddedString& operator=(const int i)
			{
				char buf[10];
				snprintf(buf, 10, "%d", i);
				strcpy(mData, buf);
				mLength = strlen(buf);
				return *this;
			}

			EmbeddedString& operator=(const double d)
			{
				char buf[10];
				dtostrf(d, 4, 2, buf);
				strcpy(mData, buf);
				mLength = strlen(buf);
				return *this;
			}

			// Other operators
			constexpr operator const char* () const { return mData; }

			void operator+= (const char* m) { concat(m); }
			void operator+= (const EmbeddedString& other) { concat(other); }
			void operator+= (const double d) { concat(d); }
			void operator+= (const int i) { concat(i); }
			void operator+= (const char c) { concat(c); }

			constexpr const char* operator+ (const char* other) const
			{
				char ret[capacity];

				strcpy(ret, mData);
				strcat(ret, other);
				return ret;
			}

			constexpr const char* operator+ (const EmbeddedString& other) const
			{
				char ret[capacity];

				strcpy(ret, mData);
				strcat(ret, other.mData);
				return ret;
			}

			constexpr bool operator== (const char* other) const
			{
				return strcmp(mData, other) == 0;
			}

			constexpr bool operator== (const EmbeddedString& other) const
			{
				return strcmp(mData, other.mData) == 0;
			}

			constexpr bool operator!= (const char* other) const
			{
				return !(*this == other);
			}

			constexpr bool operator!= (const EmbeddedString& other) const
			{
				return !(*this == other);
			}

			// Concat methods
			void concat(const char* other)
			{
				strcat(mData, other);
				mLength += strlen(other);
			}

			void concat(const EmbeddedString& other) { concat(other.mData); }

			void concat(const double d)
			{
				char buf[10];
				dtostrf(d, 4, 2, buf);
				strcat(mData, buf);
				mLength += strlen(buf);
			}

			void concat(const int i)
			{
				char buf[10];
				snprintf(buf, 10, "%d", i);
				strcat(mData, buf);
				mLength += strlen(buf);
			}

			void concat(const char c)
			{
				mData[mLength] = c;
				++mLength;
				mData[mLength] = '\0';
			}

			// Other methods
			EmbeddedString substring(const unsigned int start, unsigned int length = capacity) const
			{
				EmbeddedString newStr;

				if (mLength == 0) return newStr;
				if (start + length > mLength) length = mLength - start;
				for (unsigned int i = start; i < start + length; ++i)
					newStr.concat(mData[i]);
				return newStr;
			}

			template<class T>
			T substring(const unsigned int start, unsigned int length = capacity) const
			{
				T result;
				if (mLength == 0) return result;

				if (start + length > mLength) length = mLength - start;
				for (unsigned int i = 0; i < length; ++i)
					result.concat(this->mData[start + i]);
				return result;
			}

			constexpr unsigned int indexOf(const char c, const unsigned int startIndex = 0) const
			{
				for (unsigned int i = startIndex; i < mLength; ++i)
				{
					if (mData[i] == c)
						return i;
				}

				return OOB;
			}

			constexpr char char_at(const unsigned int index) const
			{
				if (index >= mLength) return '\0';
				return mData[index];
			}

			void clear()
			{
				mLength = 0;
				strcpy(mData, "");
			}

			constexpr char last() const { return mLength > 0 ? mData[mLength - 1] : '\0'; }
			constexpr float toFloat() const { return atof(mData); }
			constexpr int toInt() const { return atoi(mData); }
			constexpr const char* c_str() const { return mData; }
			constexpr unsigned int length() const { return mLength; }
			static constexpr unsigned int size() { return capacity; }

		private:
			char mData[capacity];
			unsigned int mLength;

			static constexpr unsigned int OOB = 4294967295;
		};
	}

	using BigString = details::EmbeddedString<1000>;
	using DefaultString = details::EmbeddedString<500>;
	using SmallString = details::EmbeddedString<400>;
	using HeaderString = details::EmbeddedString<100>;
	using SplitsString = details::EmbeddedString<30>;
	using TinyString = details::EmbeddedString<60>;
}
#endif // _EmbeddedString_
