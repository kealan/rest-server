#!/usr/bin/env python

"""
Request status from RP
"""

import sys
import utils
import config
import sys
import json
import hashlib
import re
import os
import urllib
import json


if __name__ == '__main__':
    headers = {"content-type": "application/json", "Accept": "text/plain"}
    url = config.STATUS
    print "Request url {} headers {}\n".format(url, headers)
    data, response = utils.make_request(
        url, 'GET', headers, None, True, True)
    print "Response data {}\n".format(utils.formatdata(data))
