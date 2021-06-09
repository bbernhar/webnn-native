import argparse
import glob
import numpy as np
from onnx import numpy_helper
import onnx
import os
import re

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '-d',
      '--input_dir',
      help='test_data_set parent directory')
  parser.add_argument(
      '-n',
      '--model_name',
      default='model_name',
      help='model name')     
  args = parser.parse_args()

  pbfile_list = glob.glob('%s/*/*.pb' % args.input_dir)

  for pbfile in pbfile_list:
    dir_name = re.findall(r'\d+$', os.path.dirname(pbfile))[0]
    npy_name = os.path.splitext(os.path.basename(pbfile))[0]
    save_dir = 'out/%s_nchw/test_data_set/%s' % (args.model_name, dir_name)
    if not os.path.exists(save_dir):
      os.makedirs(save_dir)
    data = onnx.load_tensor(pbfile)
    converted_data = numpy_helper.to_array(data)
    with open('%s/%s.npy' % (save_dir, npy_name), 'wb') as f:
      np.save(f, converted_data)
