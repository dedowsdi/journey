#!/bin/bash

if [[ $# -eq 0 ]]; then
  echo at least 1 gl file must be specified
  exit 0
fi

grep -h -P '^#define\s+\w+\s+0x\w{4}' "$@" | \
  grep -v -P '_ARB|_EXT|_KHR' | \
  grep -v -P '_(ATI|NV|NVX|AMD|SUN|SUNX|SGI|SGIX|SGIS|PGI|WIN|HP|INGR|OML|MESA|MESAX|INTEL|IBM|SNORM|APPLE|S3TC|OVR|OES|3DFX)\b' | \
  cut -d ' ' -f 2 | cut -f 1 | sort | uniq | \
while read macro ; do
  # ignore items with only 1 _
  count=$(tr -d 'A-Z0-9' <<<$macro | tr -d '[:digit:]' | wc -m)
  # count includes linebreak
  if [[ $count -lt 3 ]]; then
    continue
  fi
  abbre=$(perl -pe 's/(?<=_[A-Z])[A-Z]+(?=[0-9]*(_|$))//g' <<<$macro | tr -d '_' | tr '[:upper:]' '[:lower:]')
  echo "$abbre"  "$macro"
done | \
  sort
