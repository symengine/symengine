#ifndef SYMENGINE_PARSER_ALLOC_H
#define SYMENGINE_PARSER_ALLOC_H

#define ALIGNMENT 8

inline size_t align(size_t n) {
  return (n + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

class Allocator
{
    void *start;
    size_t current_pos;
    size_t size;
public:
    Allocator(size_t s) {
        start = malloc(s);
        current_pos = (size_t)start;
        current_pos = align(current_pos);
        size = s;
    }

    void *allocate(size_t s) {
        size_t addr = current_pos;
        current_pos += align(s);
        if (current_pos - (size_t)start > size) throw std::bad_alloc();
        return (void*)addr;
    }

    template <typename T, typename... Args> T* make_new(Args &&... args) {
        return new(allocate(sizeof(T))) T(std::forward<Args>(args)...);
    }
};

#endif
