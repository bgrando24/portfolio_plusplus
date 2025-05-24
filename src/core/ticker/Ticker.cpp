/**
 * The Ticker class handles anything relevant to a specific ticker (historical prices, dividends, etc)
 */
#include "Ticker.h"
#include "../../util/Types.h"

Ticker::Ticker(char *symbol) : _symbol(symbol)
{
    this->_symbol = symbol;
};
