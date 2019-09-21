#!/usr/bin/python3

import sys

EOL = '\n  '

def pr_usage():
  print("%s <file>" % sys.argv[0])

def convert(fname):
  try:
    f = open(fname, "rb")
  except FileNotFoundError as err:
    print(err)
    return
  x = f.read()
  f.close()
  print("static const uint8_t midi_data[] = {", end = EOL)
  i = 0
  for val in x:
    if i == 15:
      eol = EOL
      i = 0
    else:
      eol = ''
      i += 1
    print("0x%02x," % val, end = eol)
  if i != 0:
    print()
  print("};")

def main():
  argc = len(sys.argv)

  if argc != 2:
    pr_usage()
    sys.exit(0)

  convert(sys.argv[1])



  sys.exit(0)

main()
