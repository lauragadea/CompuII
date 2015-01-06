import tweepy
from tweepy.cursor import Cursor
import socket
import sys
import binascii
import struct
import errno
import shlex

#resuelve el error: UnicodeEncodeError: 'ascii' codec can't encode character u'\xe1' in position 1
reload(sys)
sys.setdefaultencoding("utf-8")

# == OAuth Authentication ==
#
# This mode of authentication is the new preferred way
# of authenticating with Twitter.

# The consumer keys can be found on your application's Details
# page located at https://dev.twitter.com/apps (under "OAuth settings")
consumer_key="FHvevaEXcTMbmW9DioyiKLidg"
consumer_secret="VG46gtqxReurHjQ5WCHQnMC0ObMRLNRmu1BcdTOrTjDOJArP5a"

# The access tokens can be found on your applications's Details
# page located at https://dev.twitter.com/apps (located 
# under "Your access token")
access_token="2359468682-QpMoxaCZrw6tfbutHYwmjAFYhMdtv2VjvBXdtSn"
access_token_secret="tYuYIKP8Uau8fwucpVe9Zp19yWNADuijgGlcTHVb8c14l"

auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_token, access_token_secret)

api = tweepy.API(auth)


user = api.me()


#create an INET, STREAMing socket
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print "socket created"

#bind the socket to a public host,
serversocket.bind(('127.0.0.1', 7001))
print "socket bind complete"
#become a server socket
serversocket.listen(5)
print "socket now listening..."

#para volver a usar el puerto
#serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


while True:

	#accept connections
	#check error!!! (ver que devuelve accept)
	
	(clientsocket, address) = serversocket.accept()
	print 'Got connection from', address
	
	#Recibo el comando

	linea = clientsocket.recv(1024)
	if not linea: break
	print "lo que recibo del cliente: " + linea

	

	if (linea.startswith("timeline")):
		print "empieza con timeline"
		clientsocket.send('Thank you for connecting ' + user.name + '. Your timeline today: ')
		tweets = api.home_timeline()
		for tweet in tweets:
			clientsocket.sendall (tweet.text)
		#Guardo el comando sin los \0 que trae del recv 
		#coms = comando.split("\0")

		#for comandoStr in coms:
		#	definitivo = comandoStr
		#break
	elif (linea.startswith("tweet")):
		index = linea.find("tweet")
		comando = linea[0:index+5]
		tweet = linea[index+6:len(linea)]
		#print comando
		#print tweet
		api.update_status(tweet)
		res = "Tu tweet fue posteado"
		
		clientsocket.sendall(res)
		

		
	else:
		print "no es un comando valido"

	clientsocket.close()
	
	


	#clientsocket.send('Thank you for connecting ' + user.name + '. Obama timeline today: ')
	
	#data = clientsocket.recv(1024)
	#shows last BarackObama tweets
	#result = tweepy.Cursor(api.user_timeline, id="BarackObama").items(50)
	#for status in result:
	#	clientsocket.sendall(status.text)

	
	

serversocket.close()



