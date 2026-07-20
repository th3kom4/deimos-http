# Goal --> http server

## What should web server do?
	
### High level
	Request from browser reaches http server, it parses it and build response to send back.
	
	Static request is a HTTP GET query that asks for file on local disk of the http server.

## Features
- 


## API

### Public
	http_server::start(portno);

### Private
	parse_request(msg) --> returns name of file to find.
	find_file_on_disk(filename) --> return reference to file(dunno how it should work for now)
	send_response(file) --> send file

## Questions to research
	What kind of request comes from browser?
	What web server expected to do?

Should it just find a file to return if is is asked? For example, index.html asked to get, and
it should find it and send back, or responde with error.
