#!/usr/bin/env python3
# Copyright 2021 The WebNN-native Authors
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
import os
import sys
import tarfile
import zipfile

try:
  from urllib2 import HTTPError, URLError, urlopen
except ImportError: # For Py3 compatibility
  from urllib.error import HTTPError, URLError
  from urllib.request import urlopen

onednn_version = '2.1.0'
onednn_release_url = 'https://github.com/oneapi-src/oneDNN/releases/download/v%s' %(onednn_version[:-2])
onednn_package_names = {
    'win': 'dnnl_win_%s_cpu_vcomp.zip' %(onednn_version),
    'linux': 'dnnl_lnx_%s_cpu_gomp.tgz' %(onednn_version)}

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: download_onednn.py OS_NAME")
        sys.exit(1)

    os_name = sys.argv[1]
    thirdparty_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../third_party')
    package_name = onednn_package_names[os_name]
    package_path = os.path.join(thirdparty_path, package_name[:-4])
    file_path = os.path.join(thirdparty_path, package_name)
    url = '%s/%s' %(onednn_release_url, package_name)
    if not os.path.exists(package_path):
        if url:
            print('downloading ' + url)
            with open(file_path, 'wb') as file:
                result = urlopen(url)
                while True:
                  block = result.read(1024)
                  if not block:
                    break
                  file.write(block)

            if os.path.exists(file_path):
                print('extracting ' + file_path + ' to ' + package_path)
                if file_path[-3:] == 'zip':
                  with zipfile.ZipFile(file_path, "r") as z:
                      z.extractall(thirdparty_path)
                elif file_path[-3:] == 'tgz':
                  with tarfile.open(file_path, "r:gz") as t:
                      t.extractall(thirdparty_path)
                os.remove(file_path)
