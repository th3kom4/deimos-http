FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	build-essential \
	cmake \
	make \
	g++ \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app
COPY . .

RUN mkdir -p build && \
	cd build && \
	cmake .. && \
	make -j$(nproc)

FROM ubuntu:24.04 AS runtime

RUN useradd -m appuser
USER appuser
WORKDIR /home/appuser

COPY --from=builder /usr/src/app/build/server .
COPY --from=builder /usr/src/app/public ./public

EXPOSE 8080
ENV STATIC_DIR=./public

CMD ["./server", "8080"]
