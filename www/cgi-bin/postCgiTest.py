#!/usr/bin/env python3
"""
CGI Test Script - Sends a file via multipart form data to test a server endpoint.
Usage: python cgi_test.py <url> <file_path> [field_name] [key=value ...]
"""

import sys
import os
import mimetypes
import urllib.request
import urllib.error
import uuid


def encode_multipart_form_data(fields: dict, files: dict) -> tuple:
    """
    Encode fields and files into multipart/form-data format.

    :param fields: dict of form fields {name: value}
    :param files: dict of files {field_name: (filename, file_bytes, content_type)}
    :return: (body_bytes, content_type_header)
    """
    boundary = uuid.uuid4().hex
    lines = []

    # Add regular fields
    for name, value in fields.items():
        lines.append(f"--{boundary}".encode())
        lines.append(f'Content-Disposition: form-data; name="{name}"'.encode())
        lines.append(b"")
        lines.append(value.encode())

    # Add files
    for field_name, (filename, file_bytes, content_type) in files.items():
        lines.append(f"--{boundary}".encode())
        lines.append(
            f'Content-Disposition: form-data; name="{field_name}"; filename="{filename}"'.encode()
        )
        lines.append(f"Content-Type: {content_type}".encode())
        lines.append(b"")
        lines.append(file_bytes)

    lines.append(f"--{boundary}--".encode())
    lines.append(b"")

    body = b"\r\n".join(lines)
    content_type = f"multipart/form-data; boundary={boundary}"
    return body, content_type


def send_file_as_cgi(url: str, file_path: str, field_name: str = "file", extra_fields: dict = None):
    """
    Send a file to a CGI endpoint via POST multipart/form-data.
    """
    if not os.path.isfile(file_path):
        print(f"[ERROR] File not found: {file_path}")
        sys.exit(1)

    filename = os.path.basename(file_path)
    mime_type, _ = mimetypes.guess_type(file_path)
    if mime_type is None:
        mime_type = "application/octet-stream"

    with open(file_path, "rb") as f:
        file_bytes = f.read()

    files = {
        field_name: (filename, file_bytes, mime_type)
    }

    fields = extra_fields or {}

    body, content_type = encode_multipart_form_data(fields, files)

    print(f"[INFO] Sending POST to : {url}")
    print(f"[INFO] File            : {file_path} ({len(file_bytes)} bytes)")
    print(f"[INFO] MIME type       : {mime_type}")
    print(f"[INFO] Field name      : {field_name}")
    if fields:
        print(f"[INFO] Extra fields    : {fields}")
    print("-" * 60)

    req = urllib.request.Request(
        url,
        data=body,
        method="POST",
        headers={
            "Content-Type": content_type,
            "Content-Length": str(len(body)),
        },
    )

    try:
        with urllib.request.urlopen(req) as response:
            status = response.status
            reason = response.reason
            headers = dict(response.headers)
            response_body = response.read()

        print(f"[RESPONSE] Status  : {status} {reason}")
        print("[RESPONSE] Headers :")
        for k, v in headers.items():
            print(f"  {k}: {v}")
        print("\n[RESPONSE] Body :")
        try:
            print(response_body.decode("utf-8"))
        except UnicodeDecodeError:
            print(f"<binary data: {len(response_body)} bytes>")

    except urllib.error.HTTPError as e:
        print(f"[HTTP ERROR] {e.code} {e.reason}")
        try:
            print(e.read().decode("utf-8"))
        except Exception:
            pass
        sys.exit(1)

    except urllib.error.URLError as e:
        print(f"[CONNECTION ERROR] {e.reason}")
        sys.exit(1)


def print_usage():
    print("""
Usage:
  python cgi_test.py <url> <file_path> [field_name] [key=value ...]

Arguments:
  url         The CGI endpoint URL       e.g. http://localhost/cgi-bin/upload.cgi
  file_path   Path to the file to upload e.g. ./report.pdf
  field_name  Form field name for file   (default: "file")
  key=value   Extra form fields          e.g. user=alice token=abc123

Examples:
  python cgi_test.py http://localhost/cgi-bin/upload.cgi ./test.txt
  python cgi_test.py http://localhost/cgi-bin/upload.cgi ./photo.jpg image
  python cgi_test.py http://localhost/cgi-bin/upload.cgi ./data.csv file user=alice token=secret
""")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print_usage()
        sys.exit(1)

    target_url  = sys.argv[1]
    target_file = sys.argv[2]

    # 3rd arg is field name only if it doesn't look like key=value
    file_field = "file"
    arg_start  = 3
    if len(sys.argv) > 3 and "=" not in sys.argv[3]:
        file_field = sys.argv[3]
        arg_start  = 4

    # Remaining args are extra form fields
    extra = {}
    for arg in sys.argv[arg_start:]:
        if "=" in arg:
            k, v = arg.split("=", 1)
            extra[k] = v
        else:
            print(f"[WARN] Ignoring unrecognised argument: {arg}")

    send_file_as_cgi(target_url, target_file, field_name=file_field, extra_fields=extra)