#include <vector>

namespace util {
  template<typename T>
  void swap_remove(std::vector<T>& data, int index) {
    int last = data.size() - 1;

    data[index] = data[last];
    data.resize(last);
  }

  template<typename A>
  struct deferred_action {
    A act;

    deferred_action(A a): act(a) {}

    ~deferred_action() { 
      act(); 
    }
  };

  template<typename A>
  deferred_action<A> defer(A action)
  {
    return deferred_action<A>(action);
  }
}
