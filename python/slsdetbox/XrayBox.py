import sys
import subprocess
import logging

logger = logging.getLogger()


class XrayBox:
    def __init__(self, server = None, verbose = False):
        self.server = server
        self.verbose = verbose
        print(f'XrayBox.server: {self.server}')
        print(f'XrayBox.verbose: {self.verbose}')

    def _call(self, *args):
        """
        Default function to handle command line calls
        Waits for process to return then returns the output
        as a list using a CompletedProcess object
        """
        args = [self._client, *args]
        if self.server:
            args.append('--server')
            args.append(self.server)
        logger.debug(args)
        if self.verbose:
            print(args)
        return subprocess.run(args, stdout=subprocess.PIPE, encoding="UTF-8").stdout

    @property
    def HV(self):
        """
        Check or switch on/off the high voltage of the xray tube.

        
        Examples
        ----------
        
        ::
            
            box.HV
            >> True
        
            box.HV = False
            
        """
        out = self._call("getHV")
        value = out.split()[-1]
        if value == "on":
            return True
        elif value == "off":
            return False
        else:
            raise ValueError("Could not see if HV is on")
        return out

    @HV.setter
    def HV(self, value):
        if value is True:
            logger.info("Switching on HV")
            out = self._call("HV", "on")
        else:
            logger.info("Switching off HV")
            out = self._call("HV", "off")

    @property
    def voltage(self):
        """
        High voltage of the X-ray tube in kV
        
        ::
            
            xray_box.voltage = 60
            
            xray_box.voltage
            >> 60
            
        """
        out = self._call("getActualV")
        kV = int(out.split()[-2])
        logger.info(f"Voltage is {kV} kV")
        return kV

    @voltage.setter
    def voltage(self, kV):
        logger.info("Setting HV to %f kV", kV)
        self._call("setv", str(kV))

    @property
    def warmup_time(self):
        """
        Read the warmuptime left, returns 0 if warmup not in progress
        """
        out = self._call("getwtime")
        return int(out.split()[-2])

    @property
    def current(self):
        """
        Tube current in mA
        
        ::
            
            xray_box.current = 40
            
            xray_box.current
            >> 40
        
        """
        out = self._call("getActualC")
        mA = int(out.split()[-2])
        logger.info(f"Current is {mA} mA")
        return mA

    @current.setter
    def current(self, mA):
        logger.info(f"Setting current to: {mA} mA")
        self._call("setc", str(mA))

    @property
    def shutters(self):
        out = self._call("getshutters")
        sh = out.split('\n')[1].split()
        return sh

    @property
    def error(self):
        out = self._call("geterr")
        ec = int(out.split('\n')[1])
        return ec

    @property
    def error_msg(self):
        out = self._call("getemess")
        msg = out.split('\n')[1]
        return msg

    def clear_error(self):
        out = self._call("clear")
        #potential answers?

    def start_warmup(self, target_voltage):
        out = self._call("warmup", f"{target_voltage}")

    @property
    def warmup_time(self):
        out = self._call("getwtime")
        wt = out.split('\n')[1].split()[0]
        wt = int(wt)
        return wt

class BigXrayBox(XrayBox):
    def __init__(self, server = None, verbose = False):
        self._client = "xrayClient"
        self._motor = "Fluorescence"
        super().__init__(server, verbose)

    @property
    def target(self):
        out = self._call("getfl", self._motor)
        return out.split('\n')[1]

    @target.setter
    def target(self, pos):
        self._call("setfl", self._motor, str(pos))

    def xrf_open(self):
        self._call("shutter", "1", "on")

    def xrf_close(self):
        self._call("shutter", "1", "off")

