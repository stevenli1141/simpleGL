#!/usr/bin/python
import os
import sys

s = ''
fp = sys.argv[1]
with open(fp, 'r') as f:
  s = '\\n'.join(f.read().split('\n'))

print('const char g_shader_%s_src[] = "%s";' % (os.path.basename(fp).split('.')[0], s))
