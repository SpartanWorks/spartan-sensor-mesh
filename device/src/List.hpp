#ifndef __LIST_HPP__
#define __LIST_HPP__

#include <functional>

template<typename T>
class List {
public:
  T item;
  List<T> *next = nullptr;

  List(T i, List<T> *n): item(i), next(n) {}
  ~List() {
    if (next != nullptr) {
      delete next;
      next = nullptr;
    }
  }
};

template<typename T>
void foreach(const List<T> *i, std::function<void(T)> f) {
  while (i != nullptr) {
    f(i->item);
    i = i->next;
  }
}

template<typename T>
bool contains(List<T> *list, T needle) {
  if(list == nullptr) {
    return false;
  } else {
    return list->item == needle || contains(list->next, needle);
  }
}
#endif
