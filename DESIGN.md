random design notes

# top level os/"hardware" design

OOP model, make a proper diagram for this

UI - idk

OS Class - Runs on it's own thread, which is invisible to end user. Can be tampered with from the UI.
OS design is dependent on the ins and outs of RISC V.

Computer Struct/Class
Contains memory vector, and other resources such as disks, screen etc.
And CPU class which contains harts. Multiple harts are needed to enable proper multithreading. Controlled by the OS.
OS can interrupt each core (C++ threads), via scheduler. 

each class should still function correctly regardless of changes made to any of it's connected classes.

lots of dependency injection - could also be done on init


```
os = new Os(bootProgram)

Computer.addOS(&os);
Computer.boot();
            in computer - os.setCPU(cpu);

os.loadProgram(etcc)


...
```

# cpu csr

Control Status Registers

**Needed for doing privilege modes.**

See https://docs.riscv.org/reference/isa/v20260120/priv/machine.html

Some can be 64 bits, the counter and timer registers are examples of this, so cannot be a `uint32_t arr[4096]`. Maybe a map for ease of implentation but possible perf issues??? Would also need to guard against invalid CSRs.


