#include "TickerServicePlugin.h"

// define the static member
std::shared_ptr<YFinanceProvider> TickerServicePlugin::_global_provider = nullptr;