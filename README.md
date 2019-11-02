# web-server-c
 Simple HTTP web server and host your personal web page with the server
The web server you implement will be similar to, but much simpler than, normal web servers,
such as Apache or IIS. It only responses GET requests with static content (i.e., files that can be
delivered to browsers without having to be generated, modified, or processed). It does not
respond to other requests (e.g., POST). But, it needs to respond correctly to the invalid requests
from browsers (e.g., requests for non-existent content) by generating corresponding messages
(e.g., "HTTP 404 - Not Found" error message). It also needs to support both non-persistent
connections and persistent connections.
The program should accept a port number when it is run, and should print out a usage message
(https://en.wikipedia.org/wiki/Usage_message) to instruct the user to do so if the port number is
not given in the command line. Suppose the executable file of your program is webserver.
The command line to run the program is like the following
./server 13895
In the command line above, 13895 is a port number.
Web server needs to
 accepts connections (TCP) from web browsers;
 reads in the packets that the browsers sends;
 detects the end of a request by checking the content in the packets;
 parses the request to determine the type of HTTP connection (persistent vs. non-persistent)
and the content to be served;
 prepares and sends a response to the web browser. If the request is invalid, the response
should contain the corresponding error message. When the response is received, the browser
should be able to display the content or the error message on the screen.
Refer to the lecture notes on network programming for the functions that you can use in your
program for network communication with web browsers. Refer to the lecture notes on web and 
CS288 Intensive Programming in Linux, Fall 2018, Prof. Xiaoning Ding
DO NOT DISTRIBUTE WITHOUT WRITTEN PERMISSION FROM PROF. XIAONING DING
HTTP to have some basic ideas on how a web server interact with a web browser using HTTP
protocol. To get more information, you may also refer to some online articles introducing the
HTTP protocol. Since your program needs to parse HTTP requests and generate HTTP responses,
you need to clearly know how raw HTTP requests and responses look like. Web-sniffer at
http://web-sniffer.net/ is a useful tool for you to check the formats of raw HTTP requests and
responses.
