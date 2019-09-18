#!/bin/bash
cd "$(realpath "${BASH_SOURCE[0]%/*}")/.."
ctags --options=.gutctags glc gl43 common
