#!/bin/bash

cd ui
for ui in *.ui; do
    uic "$ui">"uis/ui_${ui:0:-3}.h"
done
