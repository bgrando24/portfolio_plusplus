import yfinance as yf

def fetch_history(ticker: str, period: str = "1mo"):
    df = yf.Ticker(ticker).history(period=period)
    # convert to dict of dict
    return df.to_dict()