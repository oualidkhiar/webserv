#!/bin/bash

# Parse the query string (e.g. ?name=John)
NAME=$(echo "$QUERY_STRING" | sed 's/name=\([^&]*\).*/\1/' | sed 's/+/ /g' | sed 's/%20/ /g')

# Default if no name provided
if [ -z "$NAME" ]; then
  NAME="World"
fi

# CGI headers with \r\n
printf "Content-Type: text/html\r\n"
printf "\r\n"

# HTML output
printf "<!DOCTYPE html>\r\n"
printf "<html lang='en'>\r\n"
printf "<head>\r\n"
printf "  <meta charset='UTF-8'/>\r\n"
printf "  <title>Hello</title>\r\n"
printf "</head>\r\n"
printf "<body>\r\n"
printf "  <h1>Hello, $NAME!</h1>\r\n"
printf "</body>\r\n"
printf "</html>\r\n"