<h1 align="center"><b>WebServer</b></h1>

## Parsing

An **HTTP** request looks like this:

```
POST /upload HTTP/1.1
Host: example.com
User-Agent: curl/8.0
Accept: */*
Content-Type: text/plain
Content-Length: 20
Connection: close

this is webserver 1337
```

In its raw form, with explicit line endings:

```
POST /upload HTTP/1.1\r\n
Host: example.com\r\n
User-Agent: curl/8.0\r\n
Accept: */*\r\n
Content-Type: text/plain\r\n
Content-Length: 20\r\n
Connection: close\r\n
\r\n
this is webserver 1337
```

## Request Line

The request line is the first line of an HTTP request. It has three parts separated by a single space:

```
METHOD /path HTTP/version\r\n
```

| Part | Example | Description |
|------|---------|-------------|
| Method | `POST` | The action to perform |
| Path | `/upload` | The target resource |
| Version | `HTTP/1.1` | The HTTP protocol version |

Since this is a mini HTTP webserver, only three methods are implemented:

- **GET** — retrieve a resource from the server
- **POST** — send data to the server (e.g. form submission, file upload)
- **DELETE** — remove a resource from the server

Examples:

```http
GET /index.html HTTP/1.1\r\n
POST /upload HTTP/1.1\r\n
DELETE /file.txt HTTP/1.1\r\n
```

Any other method (e.g. `PUT`, `PATCH`) will be rejected.

## Headers

Headers follow the request line, one per line:

```
Header-Name: value\r\n
```

In more detail, as defined in [RFC 7230](https://datatracker.ietf.org/doc/html/rfc7230#section-3.2):

```
field-name ":" OWS field-value OWS\r\n
```

The headers block ends with an empty line (`\r\n\r\n`), which separates it from the body.

All headers are parsed, but only a few are used:

| Header | Example | Used for |
|--------|---------|----------|
| `Host` | `Host: example.com` | Identifying the target server |
| `Content-Type` | `Content-Type: text/plain` | Knowing how to interpret the body |
| `Content-Length` | `Content-Length: 20` | Reading the exact number of body bytes |
| `Transfer-Encoding` | `Transfer-Encoding: chunked` | Reading the body in chunks when length is unknown |

> `Content-Length` and `Transfer-Encoding: chunked` are mutually exclusive — a request uses one or the other, never both.

## Body

The body is optional. `GET` and `DELETE` requests typically have no body. `POST` requests usually carry one.

The body comes after the empty line (`\r\n\r\n`) that terminates the headers. There are three ways to determine how to read it:

### No body

If neither `Content-Length` nor `Transfer-Encoding` is present, the body is empty and the request is considered complete immediately after the headers.

`GET` and `DELETE` requests fall into this case.

### Content-Length

The body is read as exactly N bytes, where N is the value of the `Content-Length` header:

```
POST /upload HTTP/1.1\r\n
Content-Length: 20\r\n
\r\n
this is webserver 1337
```

### Transfer-Encoding: chunked

When the body size is not known upfront, it is sent in chunks:

- The body is split into one or more chunks sent sequentially
- Each chunk starts with its **size in hexadecimal**, followed by `\r\n`
- Then the **chunk data** follows
- Then another `\r\n` after the data
- The body ends with a **zero-size chunk**: `0\r\n\r\n`

```
POST /upload HTTP/1.1\r\n
Transfer-Encoding: chunked\r\n
\r\n
8\r\n
this is \r\n
e\r\n
webserver 1337\r\n
0\r\n
\r\n
```

### multipart/form-data

Used for file uploads. The body is split into parts, each separated by a boundary declared in the `Content-Type` header:

```
POST /upload HTTP/1.1\r\n
Content-Type: multipart/form-data; boundary=frontier\r\n
Content-Length: 147\r\n
\r\n
--frontier\r\n
Content-Disposition: form-data; name="file"; filename="hello.txt"\r\n
Content-Type: text/plain\r\n
\r\n
this is webserver 1337\r\n
--frontier--\r\n
```

Each part starts with `--<boundary>\r\n`, followed by its own headers, then an empty line, then its data. The body ends with `--<boundary>--`.

---

## Execution — POST

Once the request is fully parsed, the server executes it. Execution for POST goes through three stages: **location matching**, **validation**, then **body handling**.

### 1. Location Matching

After the request line is parsed, the server finds the best matching `location` block from the config using a **longest-prefix match** on the URI:

```
POST /uploads/file HTTP/1.1
        ↓
location /uploads { ... }   ✓ matched
location /         { ... }   fallback
```

If no location matches and there is no `/` fallback → **404 Not Found**.

### 2. Validation

Once a location is found, the server checks:

| Check | Failure response |
|---|---|
| `POST` not in location's `methods` list | 405 Method Not Allowed |
| URI targets a CGI file but its extension is not in `cgi_extension` | 403 Forbidden |
| No `upload_store` configured in the location | 403 Forbidden |
| Body size exceeds `client_max_body_size` | 413 Payload Too Large |

### 3. Body Handling

After validation, the path splits depending on whether the request targets a CGI script or a regular upload:

```
POST request
    │
    ├── CGI (e.g. .py, .php, .sh)
    │       Body → written to a temp file in /tmp/
    │       After body complete → fork() + execve()
    │       CGI reads body from stdin, writes response to stdout
    │       → 200 OK (or error from CGI output)
    │
    └── File upload
            │
            ├── multipart/form-data
            │       Boundary extracted from Content-Type header
            │       Each part parsed: headers → empty line → data
            │       Data written to a timestamped file in upload_store/
            │       Closed when final boundary (--boundary--) is seen
            │       → 201 Created
            │
            └── Plain body (any other Content-Type)
                    Body written directly to a timestamped file in upload_store/
                    File closed when body is fully received
                    → 201 Created
```

#### File naming

Uploaded files are saved with a timestamp-based name to avoid collisions:

```
upload_2024-11-01_14-32-05
upload_2024-11-01_14-32-05_1   ← if name already exists
upload_2024-11-01_14-32-05_2
```

#### CGI execution

The server passes the request context to the CGI script via environment variables:

| Variable | Value |
|---|---|
| `REQUEST_METHOD` | `POST` |
| `CONTENT_TYPE` | value of the `Content-Type` header |
| `CONTENT_LENGTH` | value of the `Content-Length` header |
| `QUERY_STRING` | query string from the URI (if any) |
| `SCRIPT_NAME` | the request URI |
| `SERVER_PROTOCOL` | `HTTP/1.1` |

The body is passed via **stdin**. The CGI output (stdout) becomes the response body.

If the CGI does not finish within the timeout → **504 Gateway Timeout**, child process is killed.
