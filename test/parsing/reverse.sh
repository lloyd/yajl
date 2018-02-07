#!/bin/sh

# in jajl_test, diagnostic must be enabled to print each token to stderr

# this will reverse the entire file, which fixes up the sense of trailing
# garbage and comments, but then un-reverse the actual tokens (or try to),
# it is almost a complete solution except user must reverse map key/value

# want to defeat multi-byte character processing and just treat as bytes:
# put the line "en_US ISO-8859-1" in /var/lib/locales/supported.d/local
# then run sudo locale-gen
# then run sudo update-locale LANG="en_US.ISO-8859-1" LANGUAGE="en_US:en"
# then login by console using Ctrl-Alt-F1
# then run this script
# then run sudo update-locale LANG="en_AU.UTF-8" LANGUAGE="en_AU:en"

for i in cases/*.json
do
  echo $i
  awk '{ print length, $0 }' <$i.err |sort -nr |cut -d" " -f2- >a
  rev <a >b
  paste b a |sed -e 's/\\/\\\\/g' -e 's/\//\\\//g' -e 's/\[/\\[/g' -e 's/\]/\\]/g' -e 's/\(.*\)\t\(.*\)/s\/\1\/\2\/g/' >c
  tac <$i |rev |sed -f c -e 's/{/XXXUNIQUE/g' -e 's/}/{/g' -e 's/XXXUNIQUE/}/g' -e 's/\[/XXXUNIQUE/g' -e 's/\]/[/g' -e 's/XXXUNIQUE/]/g' >rev_$i
  sed -e 's/ open /XXXUNIQUE/g' -e 's/ close / open /g' -e 's/XXXUNIQUE/ close /g' -e 's/{/XXXUNIQUE/g' -e 's/}/{/g' -e 's/XXXUNIQUE/}/g' -e 's/\[/XXXUNIQUE/g' -e 's/\]/[/g' -e 's/XXXUNIQUE/]/g' <$i.gold >rev_$i.gold
done
rm -f a b c
