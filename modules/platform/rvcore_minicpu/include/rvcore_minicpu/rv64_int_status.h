#pragma once
namespace MiniCPU_ns {
struct IntStatus {
    bool meip;
    bool msip;
    bool mtip;
    bool seip;
};
typedef IntStatus IntStatus_t;
}
