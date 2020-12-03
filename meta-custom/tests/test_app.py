import pytest
import subprocess
import os
import re


path = os.path.dirname(os.path.abspath(__file__))
app_dir = "../bin/app"
app = os.path.join(path, app_dir)

#######################################
#                                     #
#               ALIVE                 #
#                                     #
#######################################


@pytest.mark.correct
@pytest.mark.alive
def test_alive_cor():
    out = subprocess.check_output([app, '-c', 'alive', '-p'])
    assert out.decode('utf-8').rstrip() == "0x41 'A' 0x4c 'L' 0x49 'I' 0x56 'V' 0x45 'E'"


@pytest.mark.correct
def test_alive_no_print():
    out = subprocess.check_output([app, '-c', 'alive'])
    assert out.decode('utf-8').rstrip() == ''


#######################################
#                                     #
#             GET_RANDOM              #
#                                     #
#######################################


@pytest.mark.correct
@pytest.mark.get_random
def test_random_cor():
    out = subprocess.check_output([app, '-c', 'get_random', '10', '-p'])
    assert re.search("^" + "0x[0-9AaBbCcDdEeFf]+ '.*' " * 9 + "0x[0-9AaBbCcDdEeFf]+ '.*'$",out.decode('utf-8',errors='ignore').rstrip().replace('\n','')) is not None

@pytest.mark.correct
@pytest.mark.get_random
def test_random_no_print():
    out = subprocess.check_output([app, '-c', 'get_random', '255'])
    assert out.decode('utf-8').rstrip() == ''


@pytest.mark.correct
@pytest.mark.get_random
def test_get_random_pattern():
    out = subprocess.check_output([app, '-c', 'test_set_random_pattern', "16909060‬", '-p'])   # 16909060 = 0x01020304
    assert out.decode('utf-8').rstrip() == "0x53 'S' 0x65 'e' 0x74 't' 0x20 ' ' 0x70 'p' 0x61 'a' 0x74 't' 0x74 't' 0x65 'e' 0x72 'r' 0x6e 'n'"
    out = subprocess.check_output([app, '-c', 'test_set_random_mode', "1‬", '-p'])
    assert out.decode('utf-8').rstrip() == "0x52 'R' 0x61 'a' 0x6e 'n' 0x64 'd' 0x6f 'o' 0x6d 'm' 0x20 ' ' " \
    "0x6d 'm' 0x6f 'o' 0x64 'd' 0x65 'e' 0x20 ' ' 0x73 's' 0x65 'e' 0x74 't' 0x20 ' ' 0x2d '-' 0x20 ' ' 0x31 '1' 0x2e '.'"
    out = subprocess.check_output([app, '-c', 'get_random', "10", '-p'])
    assert out.decode('utf-8').rstrip() == "0x1 '' 0x2 '' 0x3 '' 0x4 '' 0x1 '' 0x2 '' 0x3 '' 0x4 '' 0x1 '' 0x2 ''"
    out = subprocess.check_output([app, '-c', 'test_set_random_mode', '0‬', '-p'])
    assert out.decode('utf-8').rstrip() == "0x52 'R' 0x61 'a' 0x6e 'n' 0x64 'd' 0x6f 'o' 0x6d 'm' 0x20 ' ' " \
    "0x6d 'm' 0x6f 'o' 0x64 'd' 0x65 'e' 0x20 ' ' 0x73 's' 0x65 'e' 0x74 't' 0x20 ' ' 0x2d '-' 0x20 ' ' 0x30 '0' 0x2e '.'"

@pytest.mark.incorrect
@pytest.mark.get_random
def test_random_too_many():
    out = subprocess.check_output([app, '-c', 'get_random', '256'])
    assert out.decode('utf-8').rstrip() == 'Max number of random bytes is 255'

@pytest.mark.incorrect
@pytest.mark.led_mode
def test_random_wrong_mode():
    out = subprocess.check_output([app, '-c', 'test_set_random_mode', '2', '-p'])
    assert out.decode('utf-8').rstrip() == "2 is not a valid random mode"

#######################################
#                                     #
#              LED_MODE               #
#                                     #
#######################################


@pytest.mark.correct
@pytest.mark.led_mode
def test_led_mode_cor():
    out = subprocess.check_output([app, '-c', 'set_led_mode', '2', '-p'])
    assert out.decode('utf-8').rstrip() == "0x53 'S' 0x65 'e' 0x74 't' 0x20 ' ' 0x6d 'm' 0x6f 'o' 0x64 'd' 0x65 'e'"
    out = subprocess.check_output([app, '-c', 'get_led_mode', '-p'])
    assert out.decode('utf-8').rstrip() == "0x32 '2'"

@pytest.mark.incorrect
@pytest.mark.led_mode
def test_led_wrong_mode():
    out = subprocess.check_output([app, '-c', 'set_led_mode', '4', '-p'])
    assert out.decode('utf-8').rstrip() == "4 is not a valid led mode"



#######################################
#                                     #
#           INCORRECT DATA            #
#                                     #
#######################################


@pytest.mark.incorrect
def test_wrong_command():
    out = subprocess.check_output([app, '-c', 'wrong_command', '4', '-p'])
    assert out.decode('utf-8').rstrip() == "wrong_command is not a command name"

@pytest.mark.incorrect
def test_wrong_option():
    out = subprocess.check_output([app, '--wrong_opt', 'wrong_command', '4', '-p'])
    assert out.decode('utf-8').rstrip() == "wrong option"

@pytest.mark.incorrect
def test_no_command_args():
    out = subprocess.check_output([app, '-c', "get_random", '-p'])
    assert out.decode('utf-8').rstrip() == "Too few arguments"

@pytest.mark.incorrect
def test_too_many_command_args():
    out = subprocess.check_output([app, '-c', "get_random", '10', '20', '-p'])
    assert out.decode('utf-8').rstrip() == "Too many arguments"