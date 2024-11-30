#ifndef TRANSFER_H
#define TRANSFER_H

#include <QTime>

#include "Move.h"

namespace sdtmaps {

struct Transfer : public Move {
    int transfer_time{};
};

} // namespace sdtmaps

#endif //TRANSFER_H
