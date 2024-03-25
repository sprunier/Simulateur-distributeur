#ifndef DBLOCK_H
#define DBLOCK_H
#include <mutex>

class DBLock {
public:
    static std::mutex mtx; // declaration
};

#endif // DBLOCK_H