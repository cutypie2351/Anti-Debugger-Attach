# Anti Debugger Attach
Anti Attach Technique against User-Mode Debuggers like x64dbg, ollydbg and more.

## Features
- Anti Debugging – blocks debuggers from attaching.
- Process Protection - A child process that protect the parent process.
- Notes of every part in the code to understand it better.

## How It Works
- Main process starts and creates a child process
- The child process trying to attach as a debugger to the parent process
- Windows allows only 1 debugger to attach for each process
- because of that if the child process attached succesfully then app can continue executing 
- If child process failed to attach the parent process, its means a debugger already attached it and a MessageBox of "Debugger Detected" appears

