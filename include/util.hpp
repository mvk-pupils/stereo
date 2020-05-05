#include <vector>

/// Mark a variable as being unused. This can be used to silence warnings.
#define UNUSED(x) (void)(x)

namespace util {
  /// Remove an element in the vector by swapping it withe the last element of
  /// the vector and shrink its size by one (1). Note that the order of the
  /// items is not preserved. 
  /// @param data A reference to the vector.
  /// @param index The index of the item to remove.
  template<typename T>
  void swap_remove(std::vector<T>& data, size_t index) {
    size_t last = data.size() - 1;
    data[index] = data[last];
    data.resize(last);
  }

  /// Runs a closure upon destruction.
  template<typename A>
  struct deferred_action {
    /// The closure.
    A act;

    /// Make a new instance of this class.
    deferred_action(A a): act(a) {}

    /// Destructor: runs the closure.
    ~deferred_action() { 
      act(); 
    }
  };

  /// Run a closure when the returned value goes out of scope.
  /// @param action The closure to run. 
  /// @returns A handle to a value, which upon destruction (when it goes out of
  ///          scope) runs the closure.
  template<typename A>
  deferred_action<A> defer(A action)
  {
    return deferred_action<A>(action);
  }
}
