#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <QScopedPointer>

class BlockChain
{
public:
    BlockChain();



private:
    struct Impl;
    QScopedPointer<Impl> d;
};

#endif // BLOCKCHAIN_H
