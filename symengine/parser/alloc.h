#ifndef SYMENGINE_PARSER_ALLOC_H
#define SYMENGINE_PARSER_ALLOC_H

inline size_t align(size_t n) {
  return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

class Allocator
{
    void *start;
    void *current_pos;
    size_t size;
public:
    Allocator(size_t s) {
        start = malloc(s);
        current_pos = start;
        current_pos = align(current_pos);
        size = s;
    }

    void *allocate(size_t size) {
        size_t addr = current_pos;
        current_pos += align(size);
        if (current_pos - start > size) throw std::bad_alloc;
        return addr;
    }

    template <typename T, typename... Args> T* make_new(Args &&... args) {
        return new(allocate(sizeof(T))) T(std::forward<Args>(args)...);
    }
}

#endif
