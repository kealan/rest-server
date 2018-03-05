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
    request_data = {
        'user': "bob",
        'email': "bob@example.com"
    }
    url = config.DATA
    print "Request url {} headers {} data {}\n".format(url, headers, utils.formatdata(request_data))
    data, response = utils.make_request(url, 'POST', headers, request_data, config.DEBUG, config.HTTP_DEBUG)
    print "Response data {}\n".format(utils.formatdata(data))
