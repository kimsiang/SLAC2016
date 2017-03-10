#!/bin/bash

for i in {1751..1758}; do
    ls gm2slac_run0$i.root > input.script
    ./ana input.script
done
