FROM python:3.11-slim

WORKDIR /app
# These packages are needed to build some dependencies
RUN apt-get update && apt-get install -y \
  gcc \
  python3-dev \
  musl-dev \
  libffi-dev \
  && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
COPY wait_for_it.sh .

RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 5005/udp

CMD ["sh", "wait_for_it.sh", "pwnsatc3:80", "--", "python", "connector.py"]
