#!/usr/bin/env python3
import os
import sys
import uuid
from datetime import datetime
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

    if content_length > 0:
        post_data = sys.stdin.read(content_length)
    else:
        post_data = sys.stdin.read()

    return parse_qs(post_data), post_data

def save_to_file(raw_data):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    upload_dir = os.path.join(script_dir, "..", "uploads")

    os.makedirs(upload_dir, exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    unique_id = uuid.uuid4().hex[:8]
    filename = f"post_{timestamp}_{unique_id}.txt"
    filepath = os.path.join(upload_dir, filename)

    with open(filepath, "w") as f:
        f.write(raw_data)

    return filepath, filename

def main():
    method = get_request_method()

    sys.stdout.write("Content-Type: text/html\r\n\r\n")

    if method == "GET":
        form = parse_get()
        path = form.get("path", [None])[0]

        if path:
            # Read and dump the file content to stdout
            if os.path.isfile(path):
                with open(path, "r") as f:
                    content = f.read()
                sys.stdout.write(content)
            else:
                sys.stdout.write(f"Error: file not found: {path}\n")
        else:
            sys.stdout.write("Error: no 'path' parameter provided\n")
        return

    # POST
    sys.stdout.write("<html>")
    sys.stdout.write("<head><title>Python CGI</title></head>")
    sys.stdout.write("<body>")

    form, raw_data = parse_post()
    filepath, filename = save_to_file(raw_data)

    print("<h1 style='color: green;'>POST Request Processed Successfully ✅</h1>")
    print(f"<p>Data saved to: <code>../uploads/{filename}</code></p>")
    print("<h2>Saved Fields:</h2>")
    print("<ul>")
    for key, values in form.items():
        for value in values:
            print(f"<li><b>{key}</b> = {value}</li>")
    print("</ul>")

    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()