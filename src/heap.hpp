#ifndef __HEAP_HPP__
#define __HEAP_HPP__

#include <cstddef>

#include "node.hpp"

struct heap
{
  heap(size_t capacity);
  ~heap();

  node * & front();
  node * const & front() const;
  bool empty() const;
  node ** data() const;
  size_t size() const;
  void pop_front();
  void insert(node * n);
  void swim(node * n);
  void print() const;
private:
  void grow();
  void swim(int pos);
  void sink(int pos);
  bool has_heap_prop() const;
  bool has_heap_prop_impl(int pos) const;
  void swap(int pos1, int pos2);
	
  node ** _data {nullptr};
  size_t _size {0};
  size_t _capacity {0};
};

#endif // __HEAP_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
