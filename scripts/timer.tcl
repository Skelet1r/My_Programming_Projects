#!/usr/bin/env tclsh
# x11echo.tcl

package require Tk

set true 1
set twentyMinutes 1200
set twoMinutes 120
set oneSecond 1000

proc timer {minutes} {
    global oneSecond
    for {set i $minutes} {$i > 0} {incr i -1} {
        puts "$i seconds left"
        after $oneSecond
        update
    }
}

proc showWindow {timeLimit} {
    global oneSecond
    
    if {[winfo exists .l]} {
        destroy .l
    }
    wm title . "Notification window"
    label .l -text "You should relax" -font {Helvetica 14 bold}
    pack .l -padx 20 -pady 20
    
    wm deiconify . 
    after [expr {$timeLimit * $oneSecond}] {wm withdraw .}
}

wm withdraw . 

while {$true} {
    timer $twentyMinutes
    showWindow $twoMinutes
    timer $twoMinutes
    #set twentyMinutes 2
    #set twoMinutes 2
}
