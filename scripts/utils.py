"""
Copyright 2014 CertiVox UK Ltd, All Rights Reserved.

The CertiVox M-Pin Client and Server Libraries are free software: you can
redistribute it and/or modify it under the terms of the BSD 3-Clause
License - http://opensource.org/licenses/BSD-3-Clause

For full details regarding our CertiVox terms of service please refer to
the following links:

  * Our Terms and Conditions -
    http://www.miracl.com/about-miracl/terms-and-conditions/

  * Our Security and Privacy -
    http://www.miracl.com/about-miracl/security-privacy/

  * Our Statement of Position and Our Promise on Software Patents -
    http://www.miracl.com/about-miracl/patents/
"""
import re
import json
import httplib
import urllib
import hashlib
import hmac


def make_url(url):
    """Adds base url to the input value if required

    Add RPA_server to the input value

    Args::

        url: url of request which might not contain base url i.e. https....

    Returns::

        url: url of full path

    Raises::

        NA

    """
    if url.startswith("/"):
        return "{0}{1}".format(RPA_server.rstrip("/"), url)
    else:
        return url


def make_request(url, method, request_headers, request_data, debug=False, http_debug=False):
    """Make a HTTP request

    Make a HTTP request to a server which responds with a JSON payload. The
    JSON data is deserialized and returned.

    Args::

        url: request url
        method: HTTP method
        request_data: data for request
        debug: Dedug the request
        http_debug: Verbose debug

    Returns::

        return_data: Deserialized JSON payload response
        response: Response data from request

    Raises::

        NA
    """
    if debug:
        print "http: request url %s" % url
        print "http: request method %s" % method

    match = re.search("//.*?/", url)
    server = match.group(0)[2:-1]
    path = url[match.end() - 1:]

    if 'https' in url:
        conn = httplib.HTTPSConnection(server)
    else:
        conn = httplib.HTTPConnection(server)

    # verbose debug
    conn.set_debuglevel(http_debug)

    if method == 'GET':
        if debug:
            print "http headers %s" % request_headers
        conn.request(method, path, headers=request_headers)
    elif method == 'FORM':
        # headers = {"Cache-Control": "no-cache", "Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}
        body = urllib.urlencode(request_data)
        if debug:
            print "http headers %s" % request_headers
            print "http: request body %s" % body
        method = 'POST'
        conn.request(method, path, body=body, headers=request_headers)
    else:
        # headers = {"Content-type": "application/json", "Accept": "text/plain"}
        body = json.dumps(request_data)
        if debug:
            print "http headers %s" % request_headers
            print "http: request body %s" % body
        conn.request(method, path, body=body, headers=request_headers)

    response = conn.getresponse()
    body = response.read()

    if body:
        try:
            return_data = json.loads(body)
        except ValueError as ex:
            if debug:
                print "ERROR: Cannot decode JSON response"
            return_data = None
    else:
        return_data = None

    if debug:
        print "http: response body %s" % body
        print "http: status %s" % response.status
        print "http: reason %s" % response.reason
        print "http: returned_data %s" % return_data
        # print "http: redirect %s" % response.getheader('Location')

    # Close connection
    conn.close()
    return return_data, response

def sign_cookie(key, data):
    """Form cookie data with hmac signature

    HMAC-SHA256 a message which is appended to the cookie data

    Args::

        key:  secret key
        data: Input data

    Returns::

        cookie: <data>.<signature>

    Raises::

    """
    message = json.dumps(data)
    signature = hmac.new(key, message.encode('utf-8'), hashlib.sha256).hexdigest()
    val = "{}.{}".format(message.encode("hex"),signature)
    return val

def verify_cookie(key, cookie):
    """Verify cookie data with hmac signature

    Checks hmac on cookie data

    Args::

        key: secret key
        cookie: <message>.<signature>

    Returns::

        data: cookie data in JSON format

    Raises::

    """        
    val = cookie.split('.')
    message = val[0].decode("hex")
    hmacMatch = hmac.new(key, message.encode('utf-8'), hashlib.sha256).hexdigest()
    hmac1 = hmac.new(key, val[1], hashlib.sha256).hexdigest()
    hmac2 = hmac.new(key, hmacMatch, hashlib.sha256).hexdigest()    
    if hmac1 != hmac2:
        return False, None
    data = json.loads(message)
    return True, data

def formatdata(jsondata):
    output = json.dumps(jsondata, sort_keys=True, indent=4, separators=(',', ': '))
    return output 

def prompt_question(
        prompt='Activated your identity via email? Setup you M-Pin now. Please enter Y/N: '):
    """Requests the user to enter Y/N to indicate that their identity has been activated

    Asks the user whether they have activated their identity by clicking
    on the email line sent by the RPA. If forceActivate is set to False
    in the RPA no such email will be set and the identity will be automatically
    activated.

    Args::

        prompt: A question asking the user to enter Y/N

    Returns::

        True / False

    Raises:
        NA
    """
    pos = ["yes", "y"]
    neg = ["no", "n"]
    while True:
        try:
            i = raw_input(prompt)
        except KeyboardInterrupt:
            return False

        if i.lower() in pos:
            return True
        elif i.lower() in neg:
            return False
        prompt = 'Please enter Y/N: '

def convert_int(input):
    """Convert a string to int.

    If the str is base ten just use int() else hex encode the input,
    convert to int and then divide by 10000

    Args::

        input: A string value

    Returns::

        ouput: A int: 0000 - 9999

    Raises::

        NA

    """
    try:
        output = int(input)
    except ValueError:
        input_hex = input.encode("hex")
        input_int = int(input_hex, 16)
        output = input_int % 10000
    return output
