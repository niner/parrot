#!./parrot
# Copyright (C) 2012, Parrot Foundation.

.sub main :main
    .local pmc task, sayer, interp, tasks
    interp = getinterp
    sayer = get_global 'sayer'
    task = new ['Task']
    setattribute task, 'code', sayer
    schedule task
    sayer(0)
.end

.sub sayer
    .local pmc things, thing
    .local int i, j

    j = j + 10
again:
    inc j
    things = new ['ResizablePMCArray']
    i = 0
next:
    thing = new 'Integer', 0
    push things, thing
    inc i
    if i > j goto end
    goto next
end:
    goto again
.end

# Local Variables:
#   mode: pir
#   fill-column: 100
# End:
# vim: expandtab shiftwidth=4 ft=pir:
