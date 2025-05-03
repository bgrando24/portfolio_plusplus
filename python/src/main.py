import yfinance as yf

abb = yf.Ticker("ABB.AX")
print(abb.history(period="1mo"))