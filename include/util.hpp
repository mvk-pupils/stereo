#include <vector>

namespace util {
  template<typename T>
  void swap_remove(std::vector<T>& data, int index) {
    int last = data.size() - 1;

    data[index] = data[last];
    data.resize(last);
  }
}
