import yfinance as yf

def fetch_history(ticker: str, period: str = "1mo"):
    df = yf.Ticker(ticker).history(period=period)
    # converrt 'DatetimeIndex' to ISO formatted string, easier to handle in C++
    df.index = df.index.strftime("%Y=%m=%dT%H:%M:%Sz")
    # convert to dict of dict
    return df.to_dict()

# # print(fetch_history("ABB.AX"))