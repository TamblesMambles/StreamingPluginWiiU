FROM ghcr.io/wiiu-env/devkitppc:20241128
COPY --from=wiiulegacy/libutils:0.1 /artifacts $DEVKITPRO/portlibs
# Get dependencies
COPY --from=wiiuwut/libutils:0.1 /artifacts $WUT_ROOT

WORKDIR project
