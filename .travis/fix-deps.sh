
# Macdeployqt don't like symlink, it uses install id to reference Frameworks (and dylib)
# When a library is found with a path as install id and referenced with a symlink to this path as link inside a lib, it fails
# I'm not sure but there might be a version related issue (eg. libavcodec.58.dylib vs libavocdec.58.X.X.dylib)
# https://bugreports.qt.io/browse/QTBUG-56814

_FFMPEG_CELLAR=$(brew list ffmpeg | grep bin/ffmpeg | sed -e "s/\/bin\/ffmpeg//")
_FFMPEG_OPT="/usr/local/opt/ffmpeg"

_FFMPEG_CELLAR_SED=$(echo $_FFMPEG_CELLAR | sed -e 's/[]\/$*.^[]/\\&/g')
_FFMPEG_OPT_SED=$(echo $_FFMPEG_OPT | sed -e 's/[]\/$*.^[]/\\&/g')

libs_to_fix=(
    /usr/local/opt/ffmpeg/lib/libavcodec.dylib
    /usr/local/opt/ffmpeg/lib/libavdevice.dylib
    /usr/local/opt/ffmpeg/lib/libavfilter.dylib
    /usr/local/opt/ffmpeg/lib/libavformat.dylib
    /usr/local/opt/ffmpeg/lib/libavresample.dylib
    /usr/local/opt/ffmpeg/lib/libswresample.dylib
    /usr/local/opt/ffmpeg/lib/libswscale.dylib
)
for lib in "${libs_to_fix[@]}"; do

    otool -L $lib | grep $_FFMPEG_CELLAR | while read -r line ; do
        install_path="$(cut -d' ' -f1 <<<"$line")"
        new_install_path="$(echo $install_path | sed -e "s/$_FFMPEG_CELLAR_SED/$_FFMPEG_OPT_SED/")"
        sudo install_name_tool -change "$install_path" "$new_install_path" "$lib"
    done

done
