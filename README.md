# Client-Server
TCP/UDP based communication

Communication protocols - Radulescu Florin

The theme is an application that respects the client-server model for management
messages. The structure of the archive containing the theme is as follows:
1) helpers.h -> file with the necessary imports for cpp files, function headers-
  and the description of the message structures designed for the protocol
  implement;
2) server.cpp -> server implementation
3) subscriber.cpp -> TCP client implementation
4) Makefile -> will run make to compile the included cpp files
5) readme.txt

Detailed description of the application's functionality starting from the included files:

1) helpers.h: - contains the imports necessary for the operation of the theme
  - contains an error checking macro (DIE), which will be widely used
    in cpp files to obstruct the propagation of errors that would
    could cause the application to stop or malfunction
  - 3 structures assigned to the messages sent in the application, as follows:
    a) tcp2srv - message sent by a TCP client to the server
    b) udp2srv - message sent by a UDP client to the server
    c) srv2tcp - message sent by the server to a TCP client
  - 2 function headers, as follows:
    a) command - function that is used by the TCP client to send to
      server a subscribe / unsubscribe command
    b) parse_message - function used by the server to transform a
      udp2srv message in a srv2tcp message

2) server.cpp, which has the following functionalities:
  - storing in memory a data structure in the form of a map containing
    the topics to which the clients are subscribed and a list of these specific clients
    daughter of each topic separately
  - creating and managing sockets and a lot of descriptor files
  - listening to sockets
  - interpreting the <exit> command from stdin with server shutdown effect and
    of all connected clients and ignoring other stdin commands
  - receiving and interpreting messages from UDP clients
  - accepting connection requests and managing the connection with TCP clients
  - receiving data from TCP clients
  - efficient transmission of incoming messages on a topic to all TCP subscribers
  - efficient error handling (TCP commands without effect, erroneous messages)
  ! server.cpp does not have the functionality of Store & Forward

3) subscriber.cpp, which has the following functionalities:
  - transforming an input into a command to the server
  - efficient management of input errors - any input error is ignored
    rate and leaves the possibility of entering another input
  - connecting to the server and sending to it the necessary data
  - sending subscribe / unsubscribe / disconnect commands to the
    ver
  - displaying messages received from the server (notifications on topics
    the customer is a subscriber)

Notes: the application works correctly for inputs or erroneous messages, a-
this does not affect the transmission of messages or the maintenance of connections. It is not im-
complemented the functionality of Store & Forward, as a result a customer will receive
messages on a topic only until it is disconnected, regardless of whether the
SF has been sent to the server. When the client is disconnected, it is deleted by
on the server any information related to it. When reconnected, it is
dad as a new customer and will not receive messages from the topics he had been to
formerly a subscriber.
