#include <list>
#include <optional>

const size_t MEMORY_SIZE = 1024;

// 内存块定义
class MemoryBlock {
 public:
  size_t start;
  size_t size;
  bool is_free;
  std::optional<size_t> job_id;
  MemoryBlock(size_t s, size_t sz, bool free, std::optional<size_t> id);
  void print();
};

class MemoryManager {
 private:
  std::list<MemoryBlock> blocks;

 public:
  MemoryManager();
  bool allocate(size_t JobId, size_t Size);

  bool free(size_t JobId);

  void print();
  std::list<MemoryBlock> getBlocks();
};
