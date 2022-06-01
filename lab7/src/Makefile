tcp: util.o
	@-rm tcpserver tcpclient
	@gcc tcpclient.c util.o -o tcpclient
	@gcc tcpserver.c util.o -o tcpserver

udp: util.o
	@-rm udpserver udpclient
	@gcc udpclient.c util.o -o udpclient
	@gcc udpserver.c util.o -o udpserver

util: util.h
	@gcc -o util.o -c util.c

clean:
	@-rm udpserver udpclient tcpserver tcpclient util.o