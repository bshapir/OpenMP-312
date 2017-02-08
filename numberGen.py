

import sys
import random

if len( sys.argv ) >= 4:
	fileName = sys.argv[1]
	rows = int( sys.argv[2] )
	cols = int( sys.argv[3] )
else:
	print 'usage: exe [output filename] [num of rows] [num of cols]'
	exit()
	
	
file = open( fileName, 'w' )

random.seed( )

file.write( str( rows ) + ' ' )
file.write( str( cols ) + "\n" )

for i in range( 0,rows ):
	for j in range( 0, cols ):
		file.write( str( random.randrange(2,100000) ) )
		file.write( ' ' )
	file.write( "\n" )


	