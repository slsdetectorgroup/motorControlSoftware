# Motor Control Software
Motor control software to control various motors for xray box, laser box and vacuum box

## Compilation
### Compile Server, Client or Gui
    # b=build, c=clean, j=number of cores (compile speed), 
    s=server, g=gui, x=xray box, l=laser box, v=vacuum box
    
    xray box server: ./cmk.sh -bcxsj9
    laser box server: ./cmk.sh -bclsj9
    vacuum box server: ./cmk.sh -bcvsj9
    xray box client and gui: ./cmk.sh -bcxgj9
    laser box client and gui: ./cmk.sh -bclgj9
    vacuum box client and gui: ./cmk.sh -bcvgj9 
        
### Binaries in build/bin
    xrayServer
    laserServer
    vacuumServer
    xrayClient_sl7
    laserClient_sl7
    vacuumClient_sl7
    XRayBoxGui_sl7
    LaserBoxGui_sl7
    VacuumBoxGui_sl7 
        
## How to Start the Server (if not already up):
    ssh l_msdetect@mpc2128
        [xraybox: mpc2128, laserbox: mpc2085,vacuumbox: mpc2069] 
    su (become root)
    killall xrayServer # kill server if it is up.
        [xrayServer, laserServer, vacuumServer] 
    cd /opt/xrayServer & # go to the folder with server installation
        [/opt/xrayServer, /opt/laserServer, /opt/vacuumServer] 
    #Check the config.txt for the right motors/ fluorescence samples
    ./xrayServer &# start server
        [xrayServer, laserServer, vacuumServer] 

## How to Start the Client:
    Go to folder /afs/psi.ch/project/sls_det_software/bin
    Run ./XRayBoxGui_sl7 or ./xrayClient_sl7 
      [LaserBoxGui_sl7, VacuumBoxGui_sl7, laserClient_sl7, vacuumClient_sl7] 

###### Note:
    The command line arguments are all case-insensitive. Error messages exist.
    The square brackets are for variables.. eg [motor name] could be 'detector_x'. 

## List of command line arguments

Tube Basic | Comments
--- | --- 
getHV | returns "on/off" for high voltage
hv [on/off] | switches on/off high voltage
getv | returns "[voltage] kV" (not actual if hv off)
setv [voltage value in kV] | sets voltage
getc | returns "[current] mA" (not actual if hv off)
setc [current value in mA] | sets current
getvc | returns "[voltage] kV, [current] mA" (not actual if hv off)
setvc [voltage value in kV] [current value in mA] | sets voltage and current
getactualv | returns "[actual voltage] kV"
getactualc | returns "[actual current] mA"
getactualvc | returns "[actual voltage] kV, [actual current] mA"
getshutter [1/2/3/4] | returns "on/off"
getshutters | returns "[on/off] [on/off] [on/off] [on/off]"
shutter [1/2/3/4] | switches "on/off" shutter [1/2/3/4]
warmup [warm-up voltage in kV] | starts the warm-up to the the voltage
getwtime | "[warm up time remaining] s"
getemess | returns error message from tube
geterr | returns error code from tube
clear | clears error code in tube

Tube Advanced | Comments
--- | --- 
gettubeinterface | returns the usb port
sendtube [Commands..] | sends tube commands
readtube [Commands..] | sends tube command and reads result from tube (beware that it should have a result)
getpower | returns "[power] W"
istube | returns "[on/off/standby]"
connectedshutters | "[c/nc] [c/nc] [c/nc] [c/nc]" nc=not connected, c=connected
issafe | returns "[yes/no]" to open door 

Motor Basic | Comments
--- | --- 
pos [motor] | returns position
cal [motor] | calibrates motor to starting hard switch and sets position to 0 
moverel [motor] [relative position] | moves motor to relative position, returns current position 
moveabs [motor] [absolute position] | moves motor to absolute position, returns current position 

Motor Advanced | Comments
--- | --- 
motorlist | list of motors
getcontroller [motor] | controller of that motor
getaxis [motor] | axis of that motor
depos [motor] | debug position
getupper [motor] | get soft upper limit
setupper [motor] [position] | set soft upper limit
getlower [motor] | get soft lower limit
setlower [motor] [position] | set soft lower limit
rangemeasure [motor] | moves motor to end hard switch to measure complete range of motor, returns final position
setpos [motor] [position] | set position of motor to that position (recommended to stick to position 0) 

Controller Advanced | Comments
--- | --- 
sendcontroller [controller] [Commands..] | send controller commands
readcontroller [controller] [Commands..] | send controller commands and read back result (beware it should have result)
controllerlist | list of controllers
getcontserialnumber [controller] | returns controller serial number
getcontinterface [controller] | returns controller serial interface
getcontmotorlist [controller] | returns list of Motors connected to its axes (- if nothing connected) 

Fluorescence Basic | Comments
--- | --- 
getholder [fluorscence motor] | get holder index
setholder [fluorscence motor] [holder index] | set holder to index
fllist [fluorscence motor] [holder index] | list of targets of holder (index) separated by space
getfl [fluorscence motor] | returns current target name
setfl [fluorscence motor] [target name] | moves motor to target name

Fluorescence Advanced | Comments
--- | --- 
numflist [fluorscence motor] | number of holders
dellastholder [fluorscence motor] | delete last holder and its targets
addholder [fluorscence motor] [Target1] [Energy 1] ..[Target8] [Energy 8] | add new holder with 8 target names and corresponding energies 

Pressure Gauges Basic | Comments
--- | --- 
pressure | "1:[[status of gauge1], [pressure of gauge 1]]\n2: [[status of gauge2], [pressure of gauge 2]]"

Pressure Gauges Advanced | Comments
--- | --- 
getpressureinterface | get pressure serial port
ispressure | returns "[on/off]" 

Reference Points Basic | Comments
--- | --- 
reflist | lists reference points
refvals [name] | x, y and z motor positions
getref | get reference point name if x, y and z at that position, else "None"
setref [name] | move x, y and z to that position (if -1, do not move)

Reference Points Advanced | Comments
--- | --- 
nref | number of reference points 

Filter Wheels Basic | Comments
--- | --- 
fwvals [name] | absoption values of that filter wheel
getfw [name] | current value of that wheel
setfw [name] [value] | move/set to that value

Filter Wheels Advanced | Comments
--- | --- 
nfw | number of filter wheels
fwlist | list of filter wheel names
getfwserialnumber [name] | serial number of that wheel
getfwinterface [name] | serial interface of that wheel 

Slits Basic | Comments
--- | --- 
getslitwidth | get slit width
setslitwidth [value] | moves both slits to match slit width
getcenter | get center of both slits
setcenter [value] | moves both slits such that center of both slits at value (based on x1) 

## Points to remember while using X-Ray Box Software:
    Client: executables can be copied and executed from anywhere. 2 types of executables:
        Command Line Arguments: ./xrayClient_sl7 followed by appropriate command line arguments.
            Your first command locks you to the server and noone else can control the server.
                If the server is locked by someone else, it will provide you with username,pcname,pid and 
                timestamp of last command sent by this user. Please check with the concerned person if 
                they are still using the xray box or decide based on timestamp. Then, you can use the 
                "unlock" command yourself to get server control.
            Use the "unlock" command when you're done using the xray box. Makes it convenient for others 
                to use it, without having to contact you. 
        GUI: ./XRayBoxGui_sl7 starts the GUI.
            Just like in xrayClient_sl7, if someone else was using the server, it provides you details and 
                gives you an option to unlock that user from server. If you choose not to unlock, it exits 
                your gui for you. If you choose to unlock, it updates you gui and gives you server control. 
                When you exit the GUI, it automatically unlocks you from the server.
            Once started, the gui will check if the X-Ray Tube is switched off/stand-by.
                If switched off, the GUI will start with the X Ray Group box disabled. This can be enabled 
                    at any time by checking the X Ray group box again.
                If it was switched on, the GUI will start with the X Ray group box displaying all the 
                    valid parameters.
                NOTE: Only when the high voltage is switched on, will it check if a warm-up is required 
                    and asks the user permission to initiate the warm-up. 
    While changing PCs (even with same username), the current PC will ask you to unlock yourself from the 
        previous PC. This is not a problem. Do that each time you change PCs while using the server. Your 
        GUI will also get updated in the process. Remember to close all your guis when you're done, 
        so that you're unlocked from server.
    Gui - X-Ray Tube Group Box:
        Checking the group box, it will check if the X-Ray Tube is on stand-by or switched off. If either 
            of these, it will display the error message and uncheck the group box and waits for the next 
            time you check it. If it was switched on, the X-Ray group box is enabled and displays all 
            the valid parameters.
        When high voltage is switched on (by clicking on the HV button):
            It evaluates if a warm-up is necessary and then asks permission from the user before initiating 
                warm-up. Warm-up can happen only when high voltage is switched on and when a new voltage is 
                set higher than the tube is warmed up to.
            Once warm-up up starts, it will display the warm-up time remaining. When warm-up is over, it 
                displays the message. However, high Voltage remains switched on after warm-up.
            The HV button turns yellow (on) or gray (off). When it ramps up to the set voltage,the actual 
                values will be displayed in red and after ramp up, it returns to a blue color.
            After ramp up (no warm up in progress), the shutter group box is enabled. Clicking any of the 
                shutters will open them and a warning icon is displayed to remind you not to open the door. 
                Opening the door when this icon is displayed will show an error message and the high voltage 
                is switched off. 
        Get error code from Tube will return error code at any point.
        To change voltage, enter a value in the appropriate field and click on "Set Voltage" button. 
            If HV is off, it changes the voltage, but checks for warm-up only when HV is switched on next. 
            If HV was already on and if warm-up is necessary, it switches off HV and back on to warm up. 
            If the warm-up wasn't necessary, it will just ramp up to the new voltage. The same goes for 
                current.
            The gui does not let you enter values greater than 60kV for voltage and greater than 
            (2400/current voltage)mA for current. If you decide to not initiate the warm-up, it will return 
            to previous operating voltage and current, even if the HV was switched on. 

