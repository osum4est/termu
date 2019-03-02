//
// Created by osum4est on 2/27/19.
//

#ifndef TERMU_INTERRUPT_HANDLER_H
#define TERMU_INTERRUPT_HANDLER_H

class interrupt_handler {
public:
    virtual void set_irq() = 0;
    virtual void set_nmi() = 0;
};

#endif //TERMU_INTERRUPT_HANDLER_H
