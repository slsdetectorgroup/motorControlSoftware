import sys
import subprocess
import logging

logger = logging.getLogger()


class XrayBox:
    def _call(self, *args):
        """
        Default function to handle command line calls
        Waits for process to return then returns the output
        as a list using a CompletedProcess object
        """
        args = (self._client, *args)
        logger.debug(args)
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
            
            xray_box.voltge = 60
            
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


class BigXrayBox(XrayBox):
    def __init__(self):
        self._client = "xrayClient"

