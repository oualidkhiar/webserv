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
    post_data = sys.stdin.read(content_length)
    return parse_qs(post_data), post_data

def save_to_file(form, raw_data):
    # Build the uploads directory path relative to this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    upload_dir = os.path.join(script_dir, "..", "uploads")

    # Create directory if it doesn't exist
    os.makedirs(upload_dir, exist_ok=True)

    # Generate a unique filename using timestamp + UUID
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    unique_id = uuid.uuid4().hex[:8]
    filename = f"post_{timestamp}_{unique_id}.txt"
    filepath = os.path.join(upload_dir, filename)

    # Write parsed fields + raw body to file
    with open(filepath, "w") as f:
        f.write(f"Timestamp : {datetime.now().isoformat()}\n")
        f.write(f"Raw POST  : {raw_data}\n")
        f.write("Fields    :\n")
        for key, values in form.items():
            for value in values:
                f.write(f"  {key} = {value}\n")

    return filepath, filename

def main():
    method = get_request_method()

    # Required CGI header
    sys.stdout.write("Content-Type: text/html\r\n\r\n")

    print("<html>")
    print("<head><title>Python CGI Test</title></head>")
    print("<body>")

    if method == "POST":
        form, raw_data = parse_post()

        # Save POST data to file
        filepath, filename = save_to_file(form, raw_data)

        print("<h1 style='color: green;'>POST Request Processed Successfully ✅</h1>")
        print(f"<p>Data saved to: <code>www/uploads/{filename}</code></p>")
        print("<h2>Saved Fields:</h2>")
        print("<ul>")
        for key, values in form.items():
            for value in values:
                print(f"<li><b>{key}</b> = {value}</li>")
        print("</ul>")

    else:
        form = parse_get()

        print("<h1 style='color: blue;'>GET Request 🔵</h1>")
        print(f"<p><strong>Request Method:</strong> {method}</p>")
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
        Age:  <input type="text" name="age"><br><br>
        <input type="submit" value="Send">
    </form>
    """)

    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()