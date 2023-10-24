23 Oct:

+ use threads
+ move inside scene
+ moves:
    key:
        up, down, left, right
    scroll:
        forward, backward
    mouse:
        rotation

+ cube3D b3aqlo
+ detect closest objects

+ modes:
    + normal mode:
    + recording mode:
        + start / stop
        + play / delete
        + frames buffer
        + move between frames buffer
        + delete frames buffer

+ optimize the code
+ slow motion move
+ build a game

create 5 threads (0,1,2,3,4)
    create 4 functions draw

    1 that update the window when all threads finish calculating : updater
    4 will do raytracing:
        + one global atomic variable each thread increment it: atm
        + set pixels buffer atomic

        if thread_index == 1 or 2 or 3 or 4
            while atm != 0
                usleep(5); // an Error may accure !!

        // check thread index : only (1,2,3,4) whom can enter the calculating loop
        thread1: will handle objects x < 0 && y > 0     start if atm = 0
        thread2: will handle objects x >= 0 && y >= 0   start if atm = 0
        thread3: will handle objects x >= 0 && y < 0    start if atm = 0
        thread4: will handle objects x < 0 && y < 0     start if atm = 0
        
        if thread_index == 1 or 2 or 3 or 4
            atm++

        while atm != 4
            usleep(5);
        
        if thread_index == 0
            atm = 0; 
            update_window

