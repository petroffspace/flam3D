#!/bin/bash
# Make the README title card flam3/flam3D-title.png: renders the four flames
# in release/title-card.flam3 with the flam3D binary in ./flam3 (run
# ./build.sh first) and lays them out with ImageMagick (convert) and the
# DejaVu Sans fonts.  The render uses a fixed seed and one thread so the
# result is the same every time.
set -e

TOP=$(cd "$(dirname "$0")/.." && pwd)
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
cd "$WORK"

FONT_DIR=/usr/share/fonts/truetype/dejavu
FB=$FONT_DIR/DejaVuSans-Bold.ttf
FR=$FONT_DIR/DejaVuSans.ttf

# 540x540 renders of the four flames: f_00000.png .. f_00003.png
env flam3_palettes="$TOP/flam3/flam3-palettes.xml" isaac_seed=flam3D \
    nthreads=1 prefix=f_ "$TOP/flam3/flam3-render" \
    < "$TOP/release/title-card.flam3" 2> render.log

W=1280; H=640           # card size
T=270; G=28; Y0=268     # tile size, gap between tiles, top of the tiles
X0=$(( (W - 4*T - 3*G) / 2 ))
cols=('#ffb347' '#ffb347' '#6fd3ff' '#6fd3ff')
caps=('2D · flam3' '2D · flam3' '3D · Apophysis 7X' '3D · Apophysis 7X')

convert -size ${W}x${H} radial-gradient:'#1a1030'-'#05050a' bg.png

# Tiles with rounded corners (radius 18)
for n in 0 1 2 3; do
   convert f_0000$n.png -resize ${T}x${T} \
      \( +clone -alpha extract \
         -draw "fill black polygon 0,0 0,18 18,0 fill white circle 18,18 18,0" \
         \( +clone -flip \) -compose Multiply -composite \
         \( +clone -flop \) -compose Multiply -composite \) \
      -alpha off -compose CopyOpacity -composite tile$n.png
done

# Title: "flam3D" in an orange-pink-blue gradient with a pink glow
convert -size 900x180 xc:none -font $FB -pointsize 150 -gravity center \
   -fill white -annotate +0+0 'flam3D' mask.png
convert \( -size 180x450 gradient:'#6fd3ff'-'#ff5fa8' \) \
        \( -size 180x450 gradient:'#ff5fa8'-'#ffb347' \) \
        -append -rotate 90 -resize 900x180\! grad.png
convert grad.png mask.png -compose CopyOpacity -composite title.png
convert title.png \( +clone -background '#ff6a8a' -shadow 60x16+0+0 \) \
   +swap -background none -layers merge +repage titleglow.png

# Each tile gets a blurred glow, a thin border and a caption
args=(bg.png)
for n in 0 1 2 3; do
   x=$((X0 + n*(T+G))); y=$Y0
   args+=( \( -size $((T+60))x$((T+60)) xc:none -fill none \
              -stroke "${cols[$n]}" -strokewidth 5 \
              -draw "roundrectangle 30,30 $((T+29)),$((T+29)) 18,18" -blur 0x12 \) \
           -geometry +$((x-30))+$((y-30)) -compose screen -composite )
   args+=( tile$n.png -geometry +$x+$y -compose over -composite )
   args+=( -fill none -stroke "${cols[$n]}99" -strokewidth 1.5 \
           -draw "roundrectangle $x,$y $((x+T-1)),$((y+T-1)) 18,18" -stroke none )
   args+=( -font $FR -pointsize 18 -fill "${cols[$n]}" \
           -draw "text $((x+4)),$((y+T+32)) '${caps[$n]}'" )
done
args+=( titleglow.png -gravity north -geometry +0-6 -compose over -composite )
args+=( -font $FR -pointsize 26 -fill '#d8d4e8' \
        -annotate +0+205 'flam3 with the Apophysis 7X 3D hack' )
args+=( -gravity southeast -font $FR -pointsize 14 -fill '#7a7590' \
        -annotate +24+14 '© petroffspace.com' )
convert "${args[@]}" -depth 8 -strip -define png:compression-level=9 \
   "$TOP/flam3/flam3D-title.png"

echo "wrote $TOP/flam3/flam3D-title.png"
