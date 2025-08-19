#!/bin/bash

TWENTY_MINUTES=$((20 * 60))   
TWO_MINUTES=$((2 * 60))       
TWO_MINUTES_MS=$((TWO_MINUTES * 1000))  

while true; do
    sleep $TWENTY_MINUTES

    notify-send -t $TWO_MINUTES_MS "You should relax"

    sleep $TWO_MINUTES
done
