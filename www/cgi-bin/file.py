#!/usr/bin/env python3

import os
import sys
from urllib.parse import parse_qs

def get_request_method():
    return os.environ.get("REQUEST_METHOD", "GET")

def parse_get():
    query_string = os.environ.get("QUERY_STRING", "")
    return parse_qs(query_string)

def parse_post():
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    except (ValueError, TypeError):
        content_length = 0

    post_data = sys.stdin.read(content_length)
    return parse_qs(post_data)

def main():
    method = get_request_method()

    if method == "POST":
        form = parse_post()
    else:
        form = parse_get()

    # Required CGI header
    sys.stdout.write("Content-Type: text/html\r\n\r\n")
    print("<html>")
    print("<head><title>Python CGI Test</title></head>")
    print("<body>")
    print("<h1>Python CGI is Working 🚀</h1>")
    print(f"<p>Request Method: {method}</p>")

    print("<h2>Received Data:</h2>")
    print("<ul>")
    for key, values in form.items():
        for value in values:
            print(f"<li><b>{key}</b> = {value}</li>")
    print("</ul>")

    print("""
    <h2>Test Form</h2>
    <form method="POST">
        Name: <input type="text" name="name"><br><br>
        Age: <input type="text" name="age"><br><br>
        <input type="submit" value="Send">
    </form>
    """)

    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()