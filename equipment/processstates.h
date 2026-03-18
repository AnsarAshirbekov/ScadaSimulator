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

enum class DamperState {
    Closed,
    Open
};

enum class DamperMode {
    Manual,
    Auto
};

#endif // PROCESSSTATES_H
