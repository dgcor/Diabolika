#!/bin/bash
mogrify -format png -transparent "rgb(255,0,255)" ./gamefilesdia2/gfx/*.bmp
for file in $(ls ./gamefilesdia2/gfx/*.png); do
  ./xbrz ./gamefilesdia2/gfx/${file##*/} 2 ./gamefilesdia2/gfx2x/${file##*/}
done
