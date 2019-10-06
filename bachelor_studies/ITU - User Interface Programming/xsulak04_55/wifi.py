#!/usr/bin/env python
import subprocess
import re
import pprint

class line_matcher:
    def __init__(self, regexp, handler):
        self.regexp  = re.compile(regexp)
        self.handler = handler


def handle_new_network(line, result, networks):
    # group(1) is the mac address
    networks.append({})
    networks[-1]['Address'] = result.group(1)

def handle_essid(line, result, networks):
    # group(1) is the essid name
    networks[-1]['ESSID'] = result.group(1)

def handle_quality(line, result, networks):
    networks[-1]['Quality'] = float(result.group(1)) / float(result.group(2))
    networks[-1]['Signal'] = int(result.group(3))

def handle_channel(line, result, networks):
    networks[-1]['Channel'] = int(result.group(1));

def handle_frequency(line, result, networks):
    networks[-1]['Frequency'] = float(result.group(1));

def handle_encryption(line, result, networks):
    on = result.group(1)
    if on == 'off':
        networks[-1]['Encryption'] = False
        networks[-1]['Authentication'] = 'No'
    elif on == 'on':
        networks[-1]['Encryption'] = True
        networks[-1]['Authentication'] = 'Unknown'

def handle_authentication(line, result, networks):
    networks[-1]['Authentication'] = result.group(1)

def handle_unknown(line, result, networks):
    # group(1) is the key, group(2) is the rest of the line
    networks[-1][result.group(1)] = result.group(2)

def scan():
    proc = subprocess.Popen(['/sbin/iwlist', 'wlp3s0', 'scan'], stdout=subprocess.PIPE)
    stdout, stderr = proc.communicate()

    stdout = stdout.decode()
    lines = stdout.split('\n')

    networks = []
    matchers = []

    # catch the line 'Cell ## - Address: XX:YY:ZZ:AA:BB:CC'
    matchers.append(line_matcher(r'\s+Cell \d+ - Address: (\S+)', handle_new_network))

    # catch the line 'ESSID:"network name"
    matchers.append(line_matcher(r'\s+ESSID:"([^"]+)"', handle_essid))

    # catch the line 'Quality=X/Y Signal level=X dBm Noise level:Y dBm'
    matchers.append(line_matcher(r'\s+Quality=(\d+)/(\d+)\s*Signal level=(-?\d+)', handle_quality))

    # catch the line 'Channel:X'
    matchers.append(line_matcher(r'\s+Channel:(\d+)', handle_channel))

    # catch the line 'Frequency:X'
    matchers.append(line_matcher(r'\s+Frequency:(\d+\.\d+)', handle_frequency))

    # catch the line 'Encryption key: X'
    matchers.append(line_matcher(r'\s+Encryption key:\s*(\S+)', handle_encryption))

    # catch the line 'IE: WPA|WPA2|WEP'
    matchers.append(line_matcher(r'\s+IE: .*(WPA2|WPA|WEP)', handle_authentication))

    # catch any other line that looks like this:
    # Key:value
    #matchers.append(line_matcher(r'\s+([^:]+):\s*(.+)', handle_unknown))

    # read each line of output, testing against the matches above
    # in that order (so that the key:value matcher will be tried last)
    for line in lines:
        for m in matchers:
            result = m.regexp.match(line)
            if result:
                m.handler(line, result, networks)
                break

    return networks


if __name__ == '__main__':
    networks = scan();
    #pprint.pprint(networks)
    for n in networks:
        print(n['ESSID'], ' ', n['Authentication'])
