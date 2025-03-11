FROM ghcr.io/wiiu-env/devkitppc:20241128
FROM ghcr.io/wiiu-env/libutilwut-f20d076
# Get dependencies
COPY --from=wiiuwut/libutils:0.1 /artifacts $WUT_ROOT

WORKDIR project
