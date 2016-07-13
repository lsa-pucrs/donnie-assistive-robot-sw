
########################################################
############# Definition of Donnie #####################



define donnie_sonar sensor
(
    # define the size of each transducer [xsize ysize zsize] in meters
    size [0.01 0.05 0.01 ] 
    # define the range bounds [min max]
    range [0.3 5.0]
    # define the angular field of view (by default in degrees)
    fov 15
    # define the color that ranges are drawn in the gui
    color_rgba [ 0.5 0.8 1 0.5]
)

define donnie_sonars ranger
( 
  # one line for each sonar [xpos ypos zpos heading]
  donnie_sonar( pose [ 0.51 0 -0.5 0])
  donnie_sonar( pose [ -0.51 0 -0.5 -180])
  donnie_sonar( pose [ 0.30 0.30 -0.5 45])
  donnie_sonar( pose [ 0.30 -0.30 -0.5 -45])
  donnie_sonar( pose [ -0.30 0.30 -0.5 -225])
  donnie_sonar( pose [ -0.30 -0.30 -0.5 225])

)

define donnie_bumper bumper
( 
	size [0.1 0.1 0.01 0]
	pose [0 0 -0.9 0]
	bcount 4  
	blength 0.75
	bpose[0] [0.5  0.35 .02  35]
	bpose[1] [0.5 -0.35 .02 -35] 
	bpose[2] [-0.5  0.35 .02 -35]
	bpose[3] [-0.5 -0.35 .02 35]
)

define donnie position
(
	# actual size
	size [1.000 1.000 1.000]
	# centre of rotation offset
	origin [0.000 0.000 0.000 0.000]
	blob_return -1

	# the shape of donnie
	block
	(
		points 8
		point[7] [0 2.8284]
		point[6] [0 6.8284]
		point[5] [2.8284 9.6568]
		point[4] [6.8284 9.6568]
		point[3] [9.6568 6.8284]
		point[2] [9.6568 2.8284]
		point[1] [6.8284 0]
		point[0] [2.8284 0]
		z [0 1]
	)
	obstacle_return 0
	#positional things
	drive "diff"


	#sensors attached to donnie
	donnie_sonars( )
	donnie_bumper( )
	#donnie_eyes( )
)

define donnie_eyes blobfinder
(
	colors_count 6
	colors ["orange" "DarkBlue" "brown" "green" "yellow" "red"]
	image [160 120] # resolution
	range 5.00 		 # meters
	fov 60 # fov (in unit_angle (degrees by def))
	size [ 0 0 0 ]
	pose [0.65 0 -1 0]
)

define donnie_HS ranger
(
	donnie_sonar( pose [ 0.51 0 -1 0])
)

define donnie_head position
(
	# actual size
	size [0.500 0.500 0.300]
	# centre of rotation offset
	origin [0.000 0.000 0.000 0.000]

	block
	(
		points 8
		point[7] [0 2.8284]
		point[6] [0 6.8284]
		point[5] [2.8284 9.6568]
		point[4] [6.8284 9.6568]
		point[3] [9.6568 6.8284]
		point[2] [9.6568 2.8284]
		point[1] [6.8284 0]
		point[0] [2.8284 0]
		z [0 1]
	)

	obstacle_return 0
	drive "diff"	

	donnie_HS()
	donnie_eyes( )
	
)


