
FROM ghcr.io/wiiu-env/devkitppc:20241128 
# Get dependencies
COPY --from=wiiuwut/libutils:0.1 /artifacts $WUT_ROOT
COPY --from=ghcr.io/wiiu-env/wiiupluginsystem:20240505 /artifacts $DEVKITPRO
WORKDIR project
