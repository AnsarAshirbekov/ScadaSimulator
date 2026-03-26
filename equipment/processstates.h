#ifndef PROCESSSTATES_H
#define PROCESSSTATES_H

enum class MotorState {
    Stopped,
    PreStart,
    Running,
    Fault
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

enum class AspirationState {
    Off,
    On,
    Cleaning
};

enum class AspirationMode {
    Manual,
    Auto
};

#endif // PROCESSSTATES_H
