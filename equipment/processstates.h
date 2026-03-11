#ifndef PROCESSSTATES_H
#define PROCESSSTATES_H

enum class ProcessState {
    Stopped,
    PreStart,
    Running
};

enum class PlowState {
    Raised,
    Lowered,
    Raising,
    Lowering,
    Stopped
};

#endif // PROCESSSTATES_H
