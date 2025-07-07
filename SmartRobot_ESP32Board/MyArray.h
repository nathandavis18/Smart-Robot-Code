#ifndef mArr
#define mArr
#pragma warning(disable : 4996)

namespace sr{
  template<class T, unsigned int capacity>
  class MyArray{
  public:
    void insert(T value){
      data[currentInsertPos] = value;
      ++currentInsertPos;
      ++numItems;
    }
    
    void remove(){
      if (numItems == 0) return;
      data[currentInsertPos].~T();
      if(currentInsertPos > 0)
        --currentInsertPos;
      --numItems;
    }

    void clear(){
      for(unsigned int i = numItems; i > 0; --i){
        remove();
      }

      currentInsertPos = 0;
      numItems = 0;
    }

    T& at(unsigned int index) { return data[index]; }

    T& operator[](unsigned int index) { return data[index]; }

    const T& at(unsigned int index) const { return data[index]; }

    const T& operator[](unsigned int index) const { return data[index]; }

    constexpr unsigned int size() const { return capacity; }

    constexpr bool operator==(const MyArray& other) const {
      if(other.size() != size()) return false;

      for(unsigned int i = 0; i < size(); ++i){
        if(this->at(i) != other.at(i)) return false;
      }

      return true;
    }

    constexpr unsigned int items() const { return numItems; }

  private:

  private:
    T data[capacity];
    unsigned int numItems = 0;
    unsigned int currentInsertPos = 0;
  };
}

#endif //mArr