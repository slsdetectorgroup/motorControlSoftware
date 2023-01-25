import pytest

server = 'localhost'

from slsdetbox import BigXrayBox

@pytest.fixture
def box():
    #Using a fixure we could detect virtual/real box
    return BigXrayBox(server = 'localhost', verbose = True)


def test_HV_off_HV_on(box):
    box.HV = False
    assert box.HV == False

    box.HV = True
    assert box.HV == True

def test_ramp_current(box):
    #TODO! for real box check values
    for c in [10,15,20]:
        box.current = c
        assert box.current == c

def test_ramp_voltage(box):
    #TODO! for real box check values
    #which current do we do this on?
    for v in [20,30,40]:
        box.voltage = v
        assert box.voltage == v

def test_error_code(box):
    assert box.error == 0

def test_error_msg(box):
    assert box.error_msg == ""