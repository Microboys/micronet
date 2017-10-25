yt target bbc-microbit-classic-gcc \
&& yt build \
&& sh fix-config.sh \
&& yt build \
&& cp build/$(yt --plain target | head -n 1 | cut -f 1 -d' ')/source/$(yt --plain ls | head -n 1 | cut -f 1 -d' ')-combined.hex "$1"
