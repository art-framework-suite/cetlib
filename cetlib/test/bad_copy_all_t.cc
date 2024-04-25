#include "cetlib/container_algorithms.h"

#include <vector>

struct noncopyable {
  noncopyable(int i) : i{i} {}
  noncopyable(noncopyable const&) = delete;
  noncopyable& operator=(noncopyable const&) = delete;
  noncopyable& operator=(noncopyable&&) = default;
  noncopyable(noncopyable&&) = default;
  int i;
};

int
main()
{
  std::vector<noncopyable> v1{}, v2;
  v1.emplace_back(noncopyable{1});
  using std::begin;
  cet::copy_all(v1, begin(v2));
}
