#include "TickerServicePlugin.h"

// Define the static member
std::shared_ptr<YFinanceProvider> TickerServicePlugin::_global_provider = nullptr;